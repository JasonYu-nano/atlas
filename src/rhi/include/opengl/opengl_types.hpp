#pragma once

#include "core_def.hpp"

#if PLATFORM_WINDOWS
#include "platform/windows/windows_minimal_api.hpp"
// place this header after the windows header.
#include "gl/gl.h"
#endif



#if PLATFORM_WINDOWS
#define OPENGL_API WINAPI*
#endif

using OPGLintptr = ptrdiff_t;
using OPGLsizeiptr = ptrdiff_t;