// Copyright 2012-2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "windowimpl.hpp"

#include "../window.hpp"
#include "../jngl/debug.hpp"

namespace jngl {
	WindowImpl::WindowImpl(Window* w) : window(w) {
		debug("init with Window: "); debug(window); debug(" this: "); debug(this); debug("\n");
	}

	void WindowImpl::setMouse(int x, int y) {
		mouseX = x;
		mouseY = y;
	}

	void WindowImpl::setMouseDown(bool d) {
		if (window->mouseDown_.at(jngl::mouse::Left)) {
			window->multitouch = d;
		}
		window->mousePressed_.at(jngl::mouse::Left) = d;
		window->mouseDown_.at(jngl::mouse::Left) = d;
	}

	void WindowImpl::setRelativeMouseMode(bool r) {
		if (r) {
			relativeX = window->mousex_;
			relativeY = window->mousey_;
			window->mousex_ = 0;
			window->mousey_ = 0;
		} else {
			relativeX = relativeY = 0;
		}
	}

	void WindowImpl::updateInput() {
		window->mousex_ = mouseX - relativeX;
		window->mousey_ = mouseY - relativeY;
		if (window->relativeMouseMode) {
			relativeX = mouseX;
			relativeY = mouseY;
		}
	}
}
