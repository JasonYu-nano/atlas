// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <limits>
#include <type_traits>

#include "core_def.hpp"
#include "check.hpp"

namespace atlas
{
namespace math
{

template<typename T>
T align(T value, uint64 alignment)
{
    static_assert(std::is_integral_v<T> || std::is_pointer_v<T>, "align expects an integer or pointer type");

    return (T)(((uint64)value + alignment - 1) & ~(alignment - 1));
}

template<typename T>
constexpr T max(const T a, const T b)
{
    static_assert(std::is_arithmetic_v<T>, "max expects an arithmetic type");
    return (a >= b) ? a : b;
}

template<typename T>
constexpr T min(const T a, const T b)
{
    static_assert(std::is_arithmetic_v<T>, "max expects an arithmetic type");
    return (a <= b) ? a : b;
}

template<typename T>
NODISCARD static constexpr T clamp(const T x, const T min, const T max)
{
    return (x < min) ? min : (x < max) ? x : max;
}

template<typename T>
constexpr T constexpr_log2_(T a, T e)
{
    return e == T(1) ? a : constexpr_log2_(a + T(1), e / T(2));
}

template<typename T>
constexpr T constexpr_log2(T t)
{
    return constexpr_log2_(T(0), t);
}

template<typename T>
constexpr bool is_aligned(T v, uint64 alignment)
{
    static_assert(std::is_integral_v<T> || std::is_pointer_v<T>, "is_aligned expects an integer or pointer type");

    return !((uint64)v & (alignment - 1));
}

template<typename To, typename From>
constexpr To cast_checked(From v)
{
    static_assert(std::is_integral_v<From> && std::is_integral_v<To>);

    if constexpr (!std::is_same_v<From, To>)
    {
        if constexpr (std::numeric_limits<From>::max() > std::numeric_limits<To>::max())
        {
            CHECK(v <= std::numeric_limits<To>::max(), "Loss of data caused by narrowing conversion");
        }
        if constexpr (std::is_signed_v<From> && !std::is_signed_v<To>)
        {
            CHECK(v > 0, "Loss of data caused by narrowing conversion");
        }
    }

    return static_cast<To>(v);
}

} // namespace math
} // namespace atlas