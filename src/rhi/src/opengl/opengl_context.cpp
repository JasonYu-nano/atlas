// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "opengl_context.hpp"
#include "platform_gl_context.hpp"
#include "opengl_functions.hpp"

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
    glfn_ = new OpenGLFunctions(*this);
}

void OpenGLContext::destroy()
{
    delete glfn_;
    glfn_ = nullptr;

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

void* OpenGLContext::get_proc_address(StringView fn_name) const
{
    return platform_context_ ? platform_context_->get_proc_address(fn_name) : nullptr;
}

OpenGLContext* OpenGLContext::current_context()
{
    return nullptr;
}

}// namespace atlas