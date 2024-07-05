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
    ::FreeLibrary(static_cast<HMODULE>(module_handle));
}

void* WindowsPlatformTraits::get_exported_symbol(void* handle, const String& symbol_name)
{
    return ::GetProcAddress(static_cast<HMODULE>(handle), symbol_name.data());
}

void WindowsPlatformTraits::set_thread_name(void* thread_handle, const String& name)
{
    typedef HRESULT(WINAPI *SetThreadDescriptionFnPtr)(HANDLE hThread, PCWSTR lpThreadDescription);

    static SetThreadDescriptionFnPtr set_thread_description = reinterpret_cast<SetThreadDescriptionFnPtr>(::GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "SetThreadDescription"));

    if (set_thread_description)
    {
        set_thread_description(thread_handle, name.to_wide().data());
    }
}

void* WindowsPlatformTraits::get_this_thread_handle()
{
    return ::GetCurrentThread();
}

ESystemMsgBoxReturnType WindowsPlatformTraits::show_message_box(ESystemMsgBoxType type, const String& caption, const String& message)
{
    switch (type)
    {
        case ESystemMsgBoxType::Ok:
        {
            MessageBox(nullptr, message.to_wide().data(), caption.to_wide().data(), MB_OK);
            return ESystemMsgBoxReturnType::Ok;
        }
        case ESystemMsgBoxType::YesNo:
        {
            int32 ret = MessageBox(nullptr, message.to_wide().data(), caption.to_wide().data(), MB_YESNO);
            return ret == IDYES ? ESystemMsgBoxReturnType::Yes : ESystemMsgBoxReturnType::No;
        }
        default:
            std::unreachable();
    }

    return ESystemMsgBoxReturnType::Ok;
}

} // namespace atlas