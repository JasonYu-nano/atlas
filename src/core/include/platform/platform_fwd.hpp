// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "core_def.hpp"

#if PLATFORM_WINDOWS
#include "platform/windows/windows_platform_traits.hpp"
#elif PLATFORM_APPLE
#include "platform/mac/mac_platform_traits.hpp"
#endif
