// Copyright 2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Include this file only once, as it defines the main function
/// \file

#pragma once

#include "App.hpp"
#include "main.hpp"
#include "screen.hpp"
#include "work.hpp"

#include <cmath>
#include <optional>

namespace jngl {
class Work;

/// Parameters used to initialize the main window
struct AppParameters {
	/// Display name of the application which will be used in the window title for example
	std::string displayName;

	/// Size of the canvas in screen pixels, see jngl::getScreenSize()
	///
	/// If not specified JNGL will create a fullscreen Window with the maximum of space available.
	std::optional<jngl::Vec2> screenSize;
};

} // namespace jngl

/// Implement this function and return a factory function which creates the first jngl::Work
std::function<std::shared_ptr<jngl::Work>()> jnglInit(jngl::AppParameters&);

#if !defined(__APPLE__) || !TARGET_OS_IPHONE // iOS
JNGL_MAIN_BEGIN {
	jngl::AppParameters params;
	auto workFactory = jnglInit(params);
	jngl::App app(params.displayName);
	if (!params.screenSize) {
		params.screenSize = { double(jngl::getDesktopWidth()), double(jngl::getDesktopHeight()) };
	}
	jngl::showWindow(params.displayName, std::lround(params.screenSize->x * jngl::getScaleFactor()),
	                 std::lround(params.screenSize->y * jngl::getScaleFactor()), true, { 1, 3 },
	                 { 3, 1 });
	jngl::setWork(workFactory());
	app.mainLoop();
}
JNGL_MAIN_END
#endif
