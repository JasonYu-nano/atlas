// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#if DEBUG
#include <cassert>
#endif

#if DEBUG
#define ASSERT(expression) assert(expression)
#else
#define ASSERT(expression)
#endif

