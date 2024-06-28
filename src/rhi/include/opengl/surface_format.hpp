// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <type_traits>

#include "core_def.hpp"
#include "core_macro.hpp"

namespace atlas
{

enum class EFormatOption : uint8
{
    StereoBuffers            = 1 << 0,
    DebugContext             = 1 << 1,
    DeprecatedFunctions      = 1 << 2,
    ResetNotification        = 1 << 3,
    ProtectedContent         = 1 << 4
};

ENUM_BIT_MASK(EFormatOption);

enum class ESwapBehavior : uint8
{
    Default,
    SingleBuffer,
    DoubleBuffer,
    TripleBuffer
};

enum class ERenderableType : uint8
{
    Default               = 0,
    OpenGL                = 1 << 0,
    OpenGLES              = 1 << 1,
    OpenVG                = 1 << 2
};

enum class EOpenGLContextProfile : uint8
{
    NoProfile,
    CoreProfile,
    CompatibilityProfile
};

class SurfaceFormat
{
public:
    SurfaceFormat() = default;
    SurfaceFormat(const SurfaceFormat&) = default;
    SurfaceFormat(SurfaceFormat&&) = default;
    SurfaceFormat& operator= (const SurfaceFormat&) = default;
    SurfaceFormat& operator= (SurfaceFormat&&) = default;

    void set_depth_buffer_size(int8 size)
    {
        depth_size_ = size;
    }
    NODISCARD int8 depth_buffer_size() const
    {
        return depth_size_;
    }
    void set_stencil_buffer_size(int8 size)
    {
        stencil_size_ = size;
    }
    NODISCARD int8 stencil_buffer_size() const
    {
        return stencil_size_;
    }
    void set_red_buffer_size(int8 size)
    {
        red_buffer_size_ = size;
    }
    NODISCARD int8 red_buffer_size() const
    {
        return red_buffer_size_;
    }
    void set_green_buffer_size(int8 size)
    {
        green_buffer_size_ = size;
    }
    NODISCARD int8 green_buffer_size() const
    {
        return green_buffer_size_;
    }
    void set_blue_buffer_size(int8 size)
    {
        blue_buffer_size_ = size;
    }
    NODISCARD int8 blue_buffer_size() const
    {
        return blue_buffer_size_;
    }
    void set_alpha_buffer_size(int8 size)
    {
        alpha_buffer_size_ = size;
    }
    NODISCARD int8 alpha_buffer_size() const
    {
        return alpha_buffer_size_;
    }
    void set_samples(int32 num_samples)
    {
        num_samples_ = num_samples;
    }
    NODISCARD int32 samples() const
    {
        return num_samples_;
    }
    NODISCARD bool has_alpha() const;

    void set_major_version(int32 majorVersion);
    NODISCARD int32 major_version() const;

    void set_minor_version(int32 minorVersion);
    NODISCARD int32 minor_version() const;

    void set_version(int32 major, int32 minor);
    NODISCARD std::pair<int32, int32> version() const;

    void set_stereo(bool enable)
    {
        enable_stereo_ = enable;
    }
    NODISCARD bool stereo() const
    {
        return enable_stereo_;
    }
    void set_options(EFormatOption options)
    {
        opts_ = options;
    }
    NODISCARD EFormatOption options() const
    {
        return opts_;
    }
    void set_swap_behavior(ESwapBehavior behavior)
    {
        swap_behavior_ = behavior;
    }
    NODISCARD ESwapBehavior swap_behavior() const
    {
        return swap_behavior_;
    }
    void set_renderable_type(ERenderableType type)
    {
        renderable_type_ = type;
    }
    NODISCARD ERenderableType renderable_type() const
    {
        return renderable_type_;
    }
    void set_profile(EOpenGLContextProfile profile)
    {
        profile_ = profile;
    }
    NODISCARD EOpenGLContextProfile profile() const
    {
        return profile_;
    }

    static void set_default_format(const SurfaceFormat& format)
    {
        if (!default_format_)
        {
            default_format_ = std::make_unique<SurfaceFormat>(format);
        }
        else
        {
            *default_format_ = format;
        }
    }

    static const SurfaceFormat& default_format()
    {
        if (!default_format_)
        {
            default_format_ = std::make_unique<SurfaceFormat>();
        }
        return *default_format_.get();
    }
private:
    int8 red_buffer_size_{ INDEX_NONE };
    int8 green_buffer_size_{ INDEX_NONE };
    int8 blue_buffer_size_{ INDEX_NONE };
    int8 alpha_buffer_size_{ INDEX_NONE };
    int8 depth_size_{ INDEX_NONE };
    int8 stencil_size_{ INDEX_NONE };
    int32 num_samples_{ 0 };
    EFormatOption opts_{ EFormatOption::StereoBuffers };
    ESwapBehavior swap_behavior_{ ESwapBehavior::Default };
    ERenderableType renderable_type_{ ERenderableType::Default };
    EOpenGLContextProfile profile_{ EOpenGLContextProfile::NoProfile };
    bool enable_stereo_{ false };

    static inline std::unique_ptr<SurfaceFormat> default_format_{ nullptr };
};

}// namespace atlas