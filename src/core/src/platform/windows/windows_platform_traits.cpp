// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "platform/windows/windows_platform_traits.hpp"
#include "platform/windows/windows_minimal_api.hpp"
#include "string/string_name.hpp"
#include "core_log.hpp"

namespace atlas
{

void* WindowsPlatformTraits::load_library(const Path& path)
{
    auto&& sys_path = path.to_os_path();
    void* handle = ::LoadLibrary(sys_path.data());
    if (handle == nullptr)
    {
        DWORD err_code = ::GetLastError();
        LOG_ERROR(core, "Load library {0} failed. error code: {1}", path.to_string(), err_code);
    }

    return handle;
}

void WindowsPlatformTraits::free_library(void* module_handle)
{
    ASSERT(module_handle);
    ::FreeLibrary((HMODULE)module_handle);
}

void* WindowsPlatformTraits::get_exported_symbol(void* handle, const String& symbol_name)
{
    return ::GetProcAddress((HMODULE)handle, symbol_name.Data());
}

} // namespace atlas
