// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "core_log.hpp"

// Ensure that the expression is true at runtime. Otherwise will abort application.
#define CHECK(expression, msg) \
    if (!(expression)) { \
        LOG_CRITICAL(core, msg); \
        std::abort(); \
    }