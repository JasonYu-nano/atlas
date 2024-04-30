// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "file_system/path.hpp"

namespace atlas
{

class CORE_API WindowsPlatformTraits
{
public:
    WindowsPlatformTraits() = delete;

    static void* LoadDynamicLibrary(const Path& path);
    static void FreeDynamicLibrary(void* module_handle);
    static void* GetExportedSymbol(void* handle, StringView symbol_name);
    static Path GetEngineDirectory();
    static Path GetDynamicLibraryPath(const Path& module_dir, const class StringName& lib_name);
};

using PlatformTraits = WindowsPlatformTraits;

} // namespace atlas