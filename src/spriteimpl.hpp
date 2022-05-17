// Copyright 2009-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "jngl/sprite.hpp"
#include "windowptr.hpp"

namespace jngl {

Finally loadSprite(const std::string&);
Sprite& GetSprite(std::string_view filename, Sprite::LoadType loadType = Sprite::LoadType::NORMAL);

extern unsigned char spriteColorRed, spriteColorGreen, spriteColorBlue, spriteColorAlpha,
                     colorRed,       colorGreen,       colorBlue,       colorAlpha;

} // namespace jngl
