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

    virtual bool MakeCurrent(const ApplicationWindow& window) = 0;
};

}// namespace atlas