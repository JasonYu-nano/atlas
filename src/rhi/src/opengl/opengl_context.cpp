// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "opengl_context.hpp"
#include "platform_gl_context.hpp"
#include "platform_gl_implementation.hpp"

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

    platform_context_ = PlatformGLImplementation::create_platform_gl_context(SurfaceFormat::default_format());
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

OpenGLFunctions* OpenGLContext::functions() const
{
    if (!glfn_)
    {
        ASSERT(platform_context_);
        glfn_ = glfn_ = new OpenGLFunctions(*this);
    }
    return glfn_;
}

OpenGLContext* OpenGLContext::current_context()
{
    return nullptr;
}

}// namespace atlas