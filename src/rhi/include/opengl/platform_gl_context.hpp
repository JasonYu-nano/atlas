// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "application_window.hpp"

namespace atlas
{

class PlatformGLContext
{
public:
    virtual ~PlatformGLContext() = default;

    NODISCARD virtual bool is_sharing() const = 0;

    NODISCARD virtual bool valid() const = 0;

    virtual bool make_current(ApplicationWindow& window) = 0;

    virtual bool swap_buffers(ApplicationWindow& window) = 0;

    virtual void done_current() = 0;

    NODISCARD virtual void* get_proc_address(StringView fn_name) const = 0;
};

}// namespace atlas