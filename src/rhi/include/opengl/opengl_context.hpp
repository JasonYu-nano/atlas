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
    OpenGLContext();

    void Create();

    void Destroy();

private:
    PlatformGLContext* platform_context_{ nullptr };
};

}// namespace atlas