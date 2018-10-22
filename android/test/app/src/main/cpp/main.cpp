#include <jngl.hpp>
#include <sstream>

JNGL_MAIN_BEGIN {
	try {
		jngl::showWindow("Android Test", 1920, 1080);
		float x = 0;
		bool first = true;
		std::string text;
		jngl::Sprite sprite("jngl.webp");
		while (jngl::running()) {
			jngl::updateInput();
			jngl::setBackgroundColor(133, 133, 133);
			jngl::setFont("Arial.ttf");
			jngl::setFontSize(100);
			if (first) {
				first = false;
				jngl::play("test.ogg");
			}
			sprite.drawClipped({0.3, 0.3}, {0.7, 0.7});
			const auto mouse = jngl::getMousePos();
			jngl::setColor(200, jngl::mouseDown(jngl::mouse::Left) ? 255 : 54, 45);
			jngl::drawRect(mouse.x, mouse.y, 100, 100);
			x += 0.4;
			// jngl::setSpriteColor(255, 255, 255, 120);
			jngl::draw("jngl.webp", mouse.x, mouse.y);
			if (jngl::mouseDown(jngl::mouse::Left)) {
				jngl::print("hello world!", 0, 0);
			}
			if (jngl::mousePressed(jngl::mouse::Left) and jngl::isMultitouch()) {
				jngl::setKeyboardVisible(true);
			}
			text += jngl::getTextInput();
			jngl::print(text, -100, -200);

			jngl::setColor(255, 0, 0);
			jngl::drawCircle({-800, -400}, 30); // top left

			jngl::setColor(255, 255, 0);
			jngl::drawCircle({800, -400}, 30); // top right

			jngl::setColor(120, 255, 120);
			jngl::drawCircle({-800, 400}, 30); // bottom left

			jngl::setColor(0, 0, 255);
			jngl::drawCircle({800, 400}, 30); // bottom right

			jngl::swapBuffers();
		}
	} catch (std::exception& e) {
		jngl::errorMessage(e.what());
	}
}
JNGL_MAIN_END
