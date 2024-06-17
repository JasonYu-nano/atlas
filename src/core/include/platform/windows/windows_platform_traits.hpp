// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "platform/generic_platform_traits.hpp"

namespace atlas
{

class CORE_API WindowsPlatformTraits : public GenericPlatformTraits
{
public:
    WindowsPlatformTraits() = delete;

    static void* load_library(const Path& path);

    static void free_library(void* module_handle);

    static void* get_exported_symbol(void* handle, const String& symbol_name);
};

using PlatformTraits = WindowsPlatformTraits;

} // namespace atlas