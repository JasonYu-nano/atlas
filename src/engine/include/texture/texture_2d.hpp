// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "core_def.hpp"
#include "texture_format_types.hpp"

namespace atlas
{

class ENGINE_API Texture2D
{
public:
    explicit Texture2D(ITextureFormat* texture) : texture_(texture) {}

    ~Texture2D()
    {
        delete texture_;
        texture_= nullptr;
    }

    NODISCARD ETextureFormat format_type() const
    {
        return texture_ ? texture_->format_type() : ETextureFormat::Unknown;
    }

private:
    ITextureFormat* texture_{ nullptr };
};

}// namespace atlas
