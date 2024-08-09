// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "assertion.hpp"
#include "color.hpp"
#include "texture_format_types.hpp"

namespace atlas
{

class ENGINE_API TFRGB8 final : public ITextureFormat
{
public:
    TFRGB8(uint32 width, uint32 height) : width_(width), height_(height)
    {
        data_ = new Color[width * height];
    }

    ~TFRGB8() override
    {
        delete data_;
        data_ = nullptr;
    }

    NODISCARD ETextureFormat format_type() const override
    {
        return ETextureFormat::RGB8;
    }

    void set_color(uint32 x, uint32 y, Color c)
    {
        ASSERT(x < width_ && y < height_ && data_);
        data_[y * width_ + x] = c;
    }

private:
    uint32 width_{ 0 };
    uint32 height_{ 0 };
    Color* data_{ nullptr };
};

}// namespace atlas