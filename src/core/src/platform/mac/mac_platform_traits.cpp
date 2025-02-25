// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <dlfcn.h>
#include <CoreFoundation/CoreFoundation.h>

#include "core_log.hpp"
#include "platform/mac/mac_platform_traits.hpp"
#include "string/string_name.hpp"
#include "utility/guid.hpp"

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

void MacPlatformTraits::set_thread_name(const String& name)
{
    pthread_setname_np(name.data());
}

void* MacPlatformTraits::get_this_thread_handle()
{
    return pthread_self();
}

void MacPlatformTraits::get_guid(GUID& guid)
{
    CFUUIDRef uuid = CFUUIDCreate(kCFAllocatorDefault);
    CFUUIDBytes uuidBytes = CFUUIDGetUUIDBytes(uuid);
    CFRelease(uuid);

    guid.a_ = (uuidBytes.byte0 << 24) | (uuidBytes.byte1 << 16) | (uuidBytes.byte2 << 8) | uuidBytes.byte3;
    guid.b_ = (uuidBytes.byte4 << 24) | (uuidBytes.byte5 << 16) | (uuidBytes.byte6 << 8) | uuidBytes.byte7;
    guid.c_ = (uuidBytes.byte8 << 24) | (uuidBytes.byte9 << 16) | (uuidBytes.byte10 << 8) | uuidBytes.byte11;
    guid.d_ = (uuidBytes.byte12 << 24) | (uuidBytes.byte13 << 16) | (uuidBytes.byte14 << 8) | uuidBytes.byte15;
}

} // namespace atlas
