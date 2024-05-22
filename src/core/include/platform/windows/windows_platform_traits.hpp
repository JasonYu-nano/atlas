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
    static void* GetExportedSymbol(void* handle, const String& symbol_name);
    static const Path& GetEngineDirectory();
    static const Path& GetRelativeBuildDirectory();
    static Path GetDynamicLibraryPath(const Path& module_dir, const class StringName& lib_name);

private:
    static Path GetEngineDirectoryImpl();
};

using PlatformTraits = WindowsPlatformTraits;

} // namespace atlas