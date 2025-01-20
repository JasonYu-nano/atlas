// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "serialize/stream.hpp"
#include "texture_format_rgb8.hpp"
#include "texture_format_types.hpp"
#include "texture_2d.gen.hpp"

namespace atlas
{

class ENGINE_API META() Texture2D
{
    GEN_CLASS_BODY(Texture2D)
public:
    Texture2D() = default;

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

    friend void serialize(WriteStream& ws, const Texture2D& texture)
    {
        texture.texture_->serialize(ws);
    }

    friend void deserialize(ReadStream& rs, Texture2D& texture)
    {
        if (texture.texture_)
        {
            texture.texture_->deserialize(rs);
        }
        else
        {
            texture.texture_ = new TFRGB8();
            texture.texture_->deserialize(rs);
        }
    }

private:
    META()
    ITextureFormat* texture_{ nullptr };
};

}// namespace atlas