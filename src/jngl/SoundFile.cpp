// Copyright 2019-2022 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "SoundFile.hpp"

#include "../Sound.hpp"
#include "../SoundParams.hpp"
#include "../audio.hpp"
#include "../main.hpp"
#include "debug.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <stdexcept>
#include <unordered_map>

#ifdef ANDROID
#include "../android/fopen.hpp"

#define AL_ALEXT_PROTOTYPES 1
#include <AL/alext.h>
#endif

#ifdef __APPLE__
// OpenAL is deprecated in favor of AVAudioEngine
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

namespace jngl {

std::unordered_map<std::string, std::shared_ptr<SoundFile>> sounds;

class Audio {
public:
	Audio() {
		device_ = alcOpenDevice(nullptr);
		if (device_ == nullptr) {
			jngl::debugLn("Could not open audio device.");
			return;
		}
		context_ = alcCreateContext(device_, nullptr);
		if (context_ == nullptr) {
			jngl::debugLn("Could not create audio context.");
			return;
		}
		alcMakeContextCurrent(context_);
	}
	Audio(const Audio&) = delete;
	Audio& operator=(const Audio&) = delete;
	Audio(Audio&&) = delete;
	Audio& operator=(Audio&&) = delete;
	~Audio() {
		sounds_.clear();
		sounds.clear();
		alcMakeContextCurrent(nullptr);
		alcDestroyContext(context_);
		alcCloseDevice(device_);
	}
	void checkAlError() {
		if (context_ == nullptr) {
			return;
		}
		switch (alGetError()) {
		case AL_NO_ERROR:
			break;
		case AL_INVALID_NAME:
			debugLn("Invalid name paramater passed to AL call.");
			break;
		case AL_INVALID_ENUM:
			debugLn("Invalid enum parameter passed to AL call.");
			break;
		case AL_INVALID_VALUE:
			debugLn("Invalid value parameter passed to AL call.");
			break;
		case AL_INVALID_OPERATION:
			debugLn("Illegal AL call.");
			break;
		case AL_OUT_OF_MEMORY:
			debugLn("Not enough memory.");
			break;
		default:
			debugLn("Unknown OpenAL error.");
		}
	}
	static bool IsStopped(std::shared_ptr<Sound>& s) {
		return s->isStopped();
	}

	void play(std::shared_ptr<Sound> sound) {
		sounds_.erase(std::remove_if(sounds_.begin(), sounds_.end(), IsStopped), sounds_.end());
		sounds_.emplace_back(std::move(sound));
	}

	void Stop(std::shared_ptr<Sound>& sound) {
		std::vector<std::shared_ptr<Sound>>::iterator i;
		if ((i = std::find(sounds_.begin(), sounds_.end(), sound)) != sounds_.end()) {
			sounds_.erase(i);
		}
	}
#ifdef ALC_SOFT_pause_device
	void pauseDevice() {
		alcDevicePauseSOFT(device_);
	}
	void resumeDevice() {
		alcDeviceResumeSOFT(device_);
	}
#endif

private:
	std::vector<std::shared_ptr<Sound>> sounds_;
	ALCdevice* device_ = nullptr;
	ALCcontext* context_ = nullptr;
};

void checkAlError() {
	GetAudio().checkAlError();
}

SoundFile::SoundFile(std::string filename, std::launch policy)
: params(std::make_unique<SoundParams>()) {
#ifndef EMSCRIPTEN
	loader = std::async(policy, [this, filename = std::move(filename)]() {
#endif
		debug("Decoding ");
		debug(filename);
		debug(" ... ");
#ifdef _WIN32
		FILE* const f = fopen(filename.c_str(), "rb");
#else
		FILE* const f = fopen(filename.c_str(), "rbe");
#endif
		if (f == nullptr) {
			throw std::runtime_error("File not found (" + filename + ").");
		}

		OggVorbis_File oggFile;
		if (ov_open(f, &oggFile, nullptr, 0) != 0) {
			fclose(f); // If [and only if] an ov_open() call fails, the application must explicitly
			           // fclose() the FILE * pointer itself.
			throw std::runtime_error("Could not open OGG file (" + filename + ").");
		}
		Finally cleanup([&oggFile]() { ov_clear(&oggFile); /* calls fclose */ });

		const vorbis_info* const pInfo = ov_info(&oggFile, -1);
		if (pInfo->channels == 1) {
			params->format = AL_FORMAT_MONO16;
		} else {
			params->format = AL_FORMAT_STEREO16;
		}
		params->freq = static_cast<ALsizei>(pInfo->rate);

		const int bufferSize = 32768;
		std::array<char, bufferSize> array{}; // 32 KB buffers
		const int endian = 0;                 // 0 for Little-Endian, 1 for Big-Endian
		int bitStream;
		long bytes; // NOLINT
		do {
			bytes = ov_read(&oggFile, &array[0], bufferSize, endian, 2, 1, &bitStream);

			if (bytes < 0) {
				throw std::runtime_error("Error decoding OGG file (" + filename + ").");
			}

			buffer_.insert(buffer_.end(), &array[0], &array[0] + bytes);
		} while (bytes > 0);

		debug("OK (");
		debug(buffer_.size() / 1024. / 1024.);
		debugLn(" MB)");
#ifndef EMSCRIPTEN
	});
#endif
}

SoundFile::~SoundFile() = default;
SoundFile::SoundFile(SoundFile&& other) noexcept {
	other.load();
	*this = std::move(other);
}
SoundFile& SoundFile::operator=(SoundFile&& other) noexcept {
#ifndef EMSCRIPTEN
	other.load();
	assert(!other.loader);
	load();
	assert(!loader);
#endif
	sound_ = std::move(other.sound_);
	params = std::move(other.params);
	buffer_ = std::move(other.buffer_);
	return *this;
}

void SoundFile::play() {
	load();
	sound_ = std::make_shared<Sound>(*params, buffer_);
	GetAudio().play(sound_);
}
void SoundFile::stop() {
	if (sound_) {
		GetAudio().Stop(sound_);
		sound_.reset();
	}
}
bool SoundFile::isPlaying() {
	if (sound_) {
		return sound_->isPlaying();
	}
	return false;
}

void SoundFile::loop() {
	if (!isPlaying()) {
		play();
	}
	sound_->loop();
}

void SoundFile::setPitch(float p) {
	if (sound_) {
		sound_->SetPitch(p);
	}
}
void SoundFile::setVolume(float v) {
	if (sound_) {
		sound_->setVolume(v);
	}
}

void SoundFile::load() {
#ifndef EMSCRIPTEN
	if (loader) {
		if (!loader->valid()) {
			throw std::runtime_error("Invalid SoundFile.");
		}
		loader->get(); // might throw
		loader = std::nullopt;
	}
#endif
}

std::shared_ptr<SoundFile> getSoundFile(const std::string& filename, std::launch policy) {
	GetAudio();
	auto i = sounds.find(filename);
	if (i == sounds.end()) { // sound hasn't been loaded yet?
		sounds[filename] = std::make_shared<SoundFile>(pathPrefix + filename, policy);
		return sounds[filename];
	}
	return i->second;
}

void play(const std::string& filename) {
	getSoundFile(filename, std::launch::deferred)->play();
}

void stop(const std::string& filename) {
	getSoundFile(filename, std::launch::async)->stop();
}

Finally loadSound(const std::string& filename) {
	auto soundFile = getSoundFile(filename, std::launch::async);
	return Finally([soundFile = std::move(soundFile)]() {
		soundFile->load();
	});
}

bool isPlaying(const std::string& filename) {
	return getSoundFile(filename, std::launch::async)->isPlaying();
}

std::shared_ptr<SoundFile> loop(const std::string& filename) {
	auto tmp = getSoundFile(filename, std::launch::deferred);
	tmp->loop();
	return tmp;
}

void setPlaybackSpeed(float speed) {
	auto end = sounds.end();
	for (auto i = sounds.begin(); i != end; ++i) {
		i->second->setPitch(speed);
	}
}

void setVolume(float volume) {
	auto end = sounds.end();
	for (auto i = sounds.begin(); i != end; ++i) {
		i->second->setVolume(volume);
	}
	Sound::masterVolume = volume;
}

#ifdef ALC_SOFT_pause_device
void pauseAudioDevice() {
	GetAudio().pauseDevice();
}

void resumeAudioDevice() {
	GetAudio().resumeDevice();
}
#endif

Audio& GetAudio() {
	static Audio audio;
	return audio;
}

} // namespace jngl
