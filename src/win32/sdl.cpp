// Copyright 2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../sdl/sdl.hpp"

#include <stdexcept>

namespace jngl {

void SDL::init() {
	static SDL sdl;
}
SDL::SDL() {
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		throw std::runtime_error(SDL_GetError());
	}
}
SDL::~SDL() {
	SDL_Quit();
}

} // namespace jngl
