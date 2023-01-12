#include "../engine.hpp"
#include "../constants.hpp"
#include "../../jngl/debug.hpp"

#include <oboe/Oboe.h>

#include <cassert>
#include <mutex>
#include <atomic>
#include <vector>
#include <cmath>

namespace psemek::audio {

struct engine::Impl {
	std::shared_ptr<stream> output;

	Impl(std::shared_ptr<stream> output);

	class Callback : public oboe::AudioStreamCallback {
	public:
		Callback(Impl& self) : self(self) {
		}

	private:
		oboe::DataCallbackResult onAudioReady(oboe::AudioStream*, void* data,
		                                      int32_t len) override {
			float* dst = reinterpret_cast<float*>(data);
            self.output->read(dst, len * 2);
			return oboe::DataCallbackResult::Continue;
		}

		Impl& self;
	};

	Callback callback;
	oboe::AudioStreamBuilder builder;
	std::shared_ptr<oboe::AudioStream> oboeStream;
};

engine::Impl::Impl(std::shared_ptr<stream> output)
: output(std::move(output)), callback(*this)
{
	builder.setDirection(oboe::Direction::Output);
	builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
	builder.setSharingMode(oboe::SharingMode::Exclusive);
	builder.setFormat(oboe::AudioFormat::Float);
	builder.setChannelCount(oboe::ChannelCount::Stereo);
	builder.setSampleRate(frequency);
	// desired.samples = 256;
	builder.setCallback(&callback);
	const auto result = builder.openStream(oboeStream);
	if (result != oboe::Result::OK) {
		throw std::runtime_error(oboe::convertToText(result));
	}

	oboeStream->requestStart();
}

engine::engine(std::shared_ptr<stream> output) : impl(std::make_unique<Impl>(std::move(output))) {
}

engine::~engine() = default;

void engine::setPause(bool pause) {
	if (pause) {
		if (impl->oboeStream) {
			impl->oboeStream->close();
			impl->oboeStream.reset();
		}
	} else {
		if (!impl->oboeStream) {
			const auto result = impl->builder.openStream(impl->oboeStream);
			if (result == oboe::Result::OK) {
				impl->oboeStream->requestStart();
			} else {
				jngl::debugLn(std::string("WARNING: ") + oboe::convertToText(result));
			}
		}
	}
}
} // namespace psemek::audio
