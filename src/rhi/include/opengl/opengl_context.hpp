// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "core_def.hpp"

namespace atlas
{

class PlatformGLContext;

class RHI_API OpenGLContext
{
public:
    OpenGLContext() = default;

    ~OpenGLContext();

    void create();

    void destroy();

    bool make_current(class ApplicationWindow& window);

    void swap_buffers(class ApplicationWindow& window);

private:
    PlatformGLContext* platform_context_{ nullptr };
};

}// namespace atlas