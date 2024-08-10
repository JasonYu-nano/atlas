// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#if DEBUG
#include <cassert>
#endif

// Ensure that the expression is true at debug mode. Equivalent to std assert.
#if DEBUG
#define ASSERT(expression) assert(expression)
#else
#define ASSERT(expression)
#endif

