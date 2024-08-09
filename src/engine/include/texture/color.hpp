// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "core_def.hpp"

namespace atlas
{

struct ENGINE_API Color
{
    union
    {
        struct { uint8 r,g,b,a; };
        uint32 components {};
    };

    Color() : r(0), g(0), b(0), a(0) {}
    Color(uint8 red, uint8 green, uint8 blue, uint8 alpha = 255) : r(red), g(green), b(blue), a(alpha) {}
    explicit Color(uint32 color) : components(color) {}
};

}// namespace atlas
