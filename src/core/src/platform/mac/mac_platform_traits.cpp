// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <dlfcn.h>

#include "platform/mac/mac_platform_traits.hpp"
#include "string/string_name.hpp"
#include "core_log.hpp"

namespace atlas
{

void* MacPlatformTraits::load_library(const Path& path)
{
    auto&& sys_path = path.to_os_path();
    void* handle = ::dlopen(sys_path.data(), RTLD_LAZY);
    if (handle == nullptr)
    {
        LOG_ERROR(core, "Load library {0} failed.", path);
    }
    return handle;
}

void MacPlatformTraits::free_library(void* module_handle)
{
    ASSERT(module_handle);
    dlclose(module_handle);
}

void* MacPlatformTraits::get_exported_symbol(void* handle, const String& symbol_name)
{
    return dlsym(handle, symbol_name.data());
}

Path MacPlatformTraits::get_library_path(const Path& module_dir, StringName lib_name)
{
#if DEBUG
    return module_dir / String::format("lib{0}d.dylib", lib_name.to_lexical());
#else
    return module_dir / String::format("lib{0}.dylib", lib_name.to_lexical());
#endif
}

} // namespace atlas
