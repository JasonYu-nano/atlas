// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "core_def.hpp"
#include "opengl_types.hpp"
#include "string/string.hpp"
#include "opengl_functions.hpp"

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

    void* get_proc_address(StringView fn_name) const;

    NODISCARD OpenGLFunctions* functions() const;

    static OpenGLContext* current_context();

private:
    PlatformGLContext* platform_context_{ nullptr };
    mutable OpenGLFunctions* glfn_{ nullptr };
};

}// namespace atlas