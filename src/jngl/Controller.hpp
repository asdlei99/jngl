// Copyright 2017 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "input.hpp"

namespace jngl {

class Controller {
public:
	virtual ~Controller() = default;

	/// Returns a value between 0.0f (not pressed) and 1.0f (pressed)
	virtual float state(controller::Button) const = 0;

	/// Returns true when the button is down
	virtual bool down(controller::Button) const = 0;

	bool pressed(controller::Button);

private:
	bool buttonPressed[jngl::controller::Last];
};

} // namespace jngl
