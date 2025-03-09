// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <memory>

namespace atlas
{

template<typename T>
using ObjectPtr = std::shared_ptr<T>;

template<typename T>
using WeakObjectPtr = std::weak_ptr<T>;

}// namespace atlas