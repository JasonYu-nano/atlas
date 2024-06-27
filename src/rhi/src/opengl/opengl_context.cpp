// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "opengl_context.hpp"
#include "platform_gl_context.hpp"

#if PLATFORM_WINDOWS
#include "platform/windows/windows_gl_context.hpp"
#endif

namespace atlas
{

OpenGLContext::~OpenGLContext()
{
    destroy();
}

void OpenGLContext::create()
{
    if (platform_context_)
    {
        destroy();
    }

    platform_context_ = new PlatformGLContextImpl(SurfaceFormat::default_format());
}

void OpenGLContext::destroy()
{
    delete platform_context_;
    platform_context_ = nullptr;
}

bool OpenGLContext::make_current(ApplicationWindow& window)
{
    if (platform_context_)
    {
        return platform_context_->make_current(window);
    }
    return false;
}

void OpenGLContext::swap_buffers(ApplicationWindow& window)
{
    if (platform_context_)
    {
        platform_context_->swap_buffers(window);
    }
}

}// namespace atlas