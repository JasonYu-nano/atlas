// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <concepts>

namespace atlas
{

template <class T>
concept LessComparable = requires(const std::remove_reference_t<T>& t1, const std::remove_reference_t<T>& t2)
{
    { t1 <  t2 } -> std::same_as<bool>;
};

}
