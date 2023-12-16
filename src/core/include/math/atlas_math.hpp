// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <type_traits>
#include "core_def.hpp"

namespace atlas
{
namespace math
{

template<typename T>
T Align(T value, uint64 alignment)
{
    static_assert(std::is_integral_v<T> || std::is_pointer_v<T>, "align expects an integer or pointer type");

    return (T)(((uint64)value + alignment - 1) & ~(alignment - 1));
}

template<typename T>
T Max(const T a, const T b)
{
    static_assert(std::is_arithmetic_v<T>, "max expects an arithmetic type");
    return (a >= b) ? a : b;
}

template<typename T>
T Min(const T a, const T b)
{
    static_assert(std::is_arithmetic_v<T>, "max expects an arithmetic type");
    return (a <= b) ? a : b;
}

template<typename T>
NODISCARD static constexpr T Clamp(const T x, const T min, const T max)
{
    return (x < min) ? min : (x < max) ? x : max;
}

} // namespace math
} // namespace atlas