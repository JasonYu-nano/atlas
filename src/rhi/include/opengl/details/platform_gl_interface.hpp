// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "core_macro.hpp"
#include "../../rhi_log.hpp"

namespace atlas
{
class SurfaceFormat;

class PlatformGLContext;

class PlatformGLInterface
{
public:
    static PlatformGLContext* create_platform_gl_context(const SurfaceFormat& request_format) VIRTUAL_IMPL(rhi, return nullptr;)

    PlatformGLInterface() = delete;
};

}// namespace atlas
