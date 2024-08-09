// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "core_def.hpp"

namespace atlas
{

enum class ETextureFormat
{
    Unknown,
    RGB8
};

class ENGINE_API ITextureFormat
{
public:
    virtual ~ITextureFormat() = default;

    NODISCARD virtual ETextureFormat format_type() const = 0;
};

}// namespace atlas