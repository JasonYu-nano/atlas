// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "assertion.hpp"
#include "color.hpp"
#include "texture_format_types.hpp"
#include "texture_format_rgb8.gen.hpp"

namespace atlas
{

class ENGINE_API META() TFRGB8 final : public ITextureFormat
{
    GEN_META_CODE(TFRGB8)
public:
    TFRGB8() = default;

    TFRGB8(uint32 width, uint32 height) : width_(width), height_(height)
    {
        data_.resize(width * height);
    }

    void serialize(WriteStream& ws) const override
    {
        ws << width_ << height_;
        for (uint32 y = 0; y < height_; ++y)
        {
            for (uint32 x = 0; x < width_; ++x)
            {
                ws << data_[y * width_ + x];
            }
        }
    }

    void deserialize(ReadStream& rs) override
    {
        rs >> width_ >> height_;

        data_.resize(width_ * height_);
        for (uint32 y = 0; y < height_; ++y)
        {
            for (uint32 x = 0; x < width_; ++x)
            {
                rs >> data_[y * width_ + x];
            }
        }
    }

    NODISCARD ETextureFormat format_type() const override
    {
        return ETextureFormat::RGB8;
    }

    void set_color(uint32 x, uint32 y, Color c)
    {
        ASSERT(x < width_ && y < height_);
        data_[y * width_ + x] = c;
    }

    private:
    META()
    uint32 width_{ 0 };

    META()
    uint32 height_{ 0 };

    META()
    Array<Color> data_;
};

}// namespace atlas