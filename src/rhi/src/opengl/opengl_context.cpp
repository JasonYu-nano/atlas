// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "opengl_context.hpp"
#include "platform_gl_context.hpp"

namespace atlas
{

void OpenGLContext::Create()
{
    if (platform_context_)
    {
        Destroy();
    }
    
}

void OpenGLContext::Destroy()
{
    delete platform_context_;
    platform_context_ = nullptr;
}

}// namespace atlas