// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "core_def.hpp"
#include "opengl_types.hpp"
#include "opengl_types.hpp"
#include "string/string.hpp"

namespace atlas
{

class PlatformGLContext;

class OpenGLFunctions;

class RHI_API OpenGLContext
{
public:
    OpenGLContext() = default;

    ~OpenGLContext();

    void create();

    void destroy();

    bool make_current(class ApplicationWindow& window);

    void swap_buffers(class ApplicationWindow& window);

    void* get_proc_address(StringView fn_name) const;

    NODISCARD OpenGLFunctions* functions() const
    {
        return glfn_;
    }

    static OpenGLContext* current_context();

private:
    PlatformGLContext* platform_context_{ nullptr };
    OpenGLFunctions* glfn_{ nullptr };
};

}// namespace atlas