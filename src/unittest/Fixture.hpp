// Copyright 2018-2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include <string>

class Fixture {
public:
	explicit Fixture(double scaleFactor);
	~Fixture();
	Fixture(const Fixture&) = delete;
	Fixture& operator=(const Fixture&) = delete;
	Fixture(Fixture&&) = delete;
	Fixture& operator=(Fixture&&) = delete;

	[[nodiscard]] std::string getAsciiArt() const;

private:
	static void reset() ;

	std::string emptyAsciiArt;
};
