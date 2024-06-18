// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "platform/generic_platform_traits.hpp"

namespace atlas
{

class CORE_API MacPlatformTraits : public GenericPlatformTraits
{
public:
    MacPlatformTraits() = delete;

    static void* load_library(const Path& path);

    static void free_library(void* module_handle);

    static void* get_exported_symbol(void* handle, const String& symbol_name);
};

using PlatformTraits = MacPlatformTraits;

} // namespace atlas