// Copyright 2018-2021 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/text.hpp"
#include "Fixture.hpp"

#include <jngl.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(CharacterTest) {
	Fixture f(2);
	jngl::Font font("../data/Arial.ttf", 40);
	jngl::Text t("m ö o ß");
	t.setFont(font);
	t.setPos(-110, -20);
	t.draw();
	const auto output = R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒           ░       ░▒         ▒
▒    ░░░░  ░▒   ░░  ▒░░        ▒
▒    ▒░▒▒  ▒ ▒ ░░▒  ▒▒         ▒
▒    ▒ ▒░  ▒ ▒ ░░▒  ▒ ▒        ▒
▒    ░ ░░  ░▒░  ▒░  ░░░        ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)";
	BOOST_CHECK_EQUAL(f.getAsciiArt(), output);
	font.print("m ö o ß", -110, -20);
	BOOST_CHECK_EQUAL(f.getAsciiArt(), output);
	font.print("m ö o ß", jngl::Vec2(-110, -20));
	BOOST_CHECK_EQUAL(f.getAsciiArt(), output);
}

BOOST_AUTO_TEST_CASE(TextTest) {
	// Test with two rather big scale factors to avoid rounding errors:
	for (double scaleFactor : { 6, 8 }) {
		Fixture f(scaleFactor);
		jngl::setFont("../data/Arial.ttf");
		jngl::Text text("test string\nline 2");
		BOOST_CHECK_CLOSE(text.getWidth(), 69.4, 0.2);
		BOOST_CHECK_EQUAL(std::lround(text.getHeight()), 38L);
		text.setCenter(-10, -10);
		BOOST_CHECK_CLOSE(text.getX(), -44.7, 0.2);
		text.draw();
		const std::string screenshotCentered = R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒          ░░▒░░░░░            ▒
▒                              ▒
▒           ░░                 ▒
▒                              ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)";
		BOOST_CHECK_EQUAL(f.getAsciiArt(), screenshotCentered);

		text.setPos(-10, -10);
		BOOST_CHECK_EQUAL(std::lround(text.getX()), -10L);
		text.draw();
		BOOST_CHECK_EQUAL(f.getAsciiArt(), R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒                  ░           ▒
▒              ░░░░░░░         ▒
▒              ░  ░            ▒
▒              ░░ ░            ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");

		text.setCenter(-10, -10); // restore previous state
		text.draw();
		BOOST_CHECK_CLOSE(text.getX(), -44.7, 0.2);
		BOOST_CHECK_EQUAL(f.getAsciiArt(), screenshotCentered);

		text.setAlign(jngl::Alignment::CENTER);
		BOOST_CHECK_CLOSE(text.getX(), -44.7, 0.2);
		text.draw(); // the second line should now be centered below the first
		BOOST_CHECK_EQUAL(f.getAsciiArt(), R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒          ░░▒░░░░░            ▒
▒               ░              ▒
▒            ░░░░              ▒
▒                              ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");

		BOOST_CHECK_EQUAL(std::lround(jngl::getTextWidth("foo")), 22);
		BOOST_CHECK_EQUAL(std::lround(jngl::getTextWidth("foo\nfoobar\nbar")), 45);
	}
}
