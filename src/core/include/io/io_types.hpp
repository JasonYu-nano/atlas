// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <span>

#include "core_def.hpp"
#include "container/array.hpp"
#include "file_system/path.hpp"

namespace atlas
{

enum class EIOPriority : uint8
{
    High,
    Normal,
    Low
};

constexpr uint8 g_io_priority_count = static_cast<uint8>(EIOPriority::Low) + 1;

using IOBuffer = Array<byte>;
using IOBufferView = std::span<byte>;

}// namespace atlas
