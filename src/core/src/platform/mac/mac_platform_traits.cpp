// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <dlfcn.h>

#include "platform/mac/mac_platform_traits.hpp"
#include "string/string_name.hpp"
#include "core_log.hpp"

namespace atlas
{

void* MacPlatformTraits::LoadDynamicLibrary(const Path& path)
{
    auto&& sys_path = path.ToOSPath();
    void* handle = ::dlopen(sys_path.data(), RTLD_LAZY);
    if (handle == nullptr)
    {
        LOG_ERROR(core_module, "Load library {0} failed.", path.ToString());
    }
    return handle;
}

void MacPlatformTraits::FreeDynamicLibrary(void* module_handle)
{
    ASSERT(module_handle);
    dlclose(module_handle);
}

void* MacPlatformTraits::GetExportedSymbol(void* handle, const String& symbol_name)
{
    return dlsym(handle, symbol_name.Data());
}

Path MacPlatformTraits::GetEngineDirectory()
{
    std::error_code error;
    std::filesystem::path working_directory = std::filesystem::current_path(error);
    String working_directory_string = String::From(working_directory);
#if DEBUG
    working_directory_string.Remove("build/debug/out/bin");
#else
    // TODO:
#endif
    return { working_directory_string };
}

Path MacPlatformTraits::GetDynamicLibraryPath(const Path& module_dir, const StringName& lib_name)
{
#if DEBUG
    return module_dir / String::Format("lib{0}d.dylib", lib_name.ToLexical());
#else
    return module_dir / String::Format("lib{0}.dylib", lib_name.ToLexical());
#endif
}


} // namespace atlas
