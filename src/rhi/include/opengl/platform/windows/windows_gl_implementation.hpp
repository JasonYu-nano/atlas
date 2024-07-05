// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "details/platform_gl_interface.hpp"
#include "windows_gl_context.hpp"

namespace atlas
{

class WindowsGLImplementation : public PlatformGLInterface
{
public:
    static PlatformGLContext* create_platform_gl_context(const SurfaceFormat& request_format)
    {
        return new WindowsGLContext(get_gl_static_context(), request_format);
    }

    WindowsGLImplementation() = delete;

private:
    static WindowGLStaticContext& get_gl_static_context()
    {
        static WindowGLStaticContext static_context;
        return static_context;
    }
};

using PlatformGLImplementation = WindowsGLImplementation;

}// namespace atlas