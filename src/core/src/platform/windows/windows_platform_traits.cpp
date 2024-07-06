// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "platform/windows/windows_platform_traits.hpp"
#include "core_log.hpp"
#include "file_system/directory.hpp"
#include "platform/windows/windows_minimal_api.hpp"
#include "string/string_name.hpp"

#if WITH_CRASH_HANDLER
#include <dbghelp.h>
#endif

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
        case ESystemMsgBoxType::Ok: {
            MessageBox(nullptr, message.to_wide().data(), caption.to_wide().data(), MB_OK);
            return ESystemMsgBoxReturnType::Ok;
        }
        case ESystemMsgBoxType::YesNo: {
            int32 ret = MessageBox(nullptr, message.to_wide().data(), caption.to_wide().data(), MB_YESNO);
            return ret == IDYES ? ESystemMsgBoxReturnType::Yes : ESystemMsgBoxReturnType::No;
        }
        default:
            std::unreachable();
    }

    return ESystemMsgBoxReturnType::Ok;
}

#if WITH_CRASH_HANDLER
static LONG exception_handler(EXCEPTION_POINTERS* exception_ptr)
{
    typedef bool (WINAPI *MiniDumpWriter)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
                    CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
                    CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);
    HMODULE library = ::LoadLibraryA("dbghelp.dll");

    MiniDumpWriter dump_writer = (MiniDumpWriter)::GetProcAddress(library, "MiniDumpWriteDump");

    const Path dir = Directory::get_engine_save_directory() / "crash_dump";
    if (!dir.exists() && !Directory::make_dir_tree(dir))
    {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    Path dump_file = dir / String::format("atals_crash_{}.dump", now_time);
    HANDLE handle = ::CreateFile(dump_file.to_os_path().data(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (handle == INVALID_HANDLE_VALUE)
    {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    MINIDUMP_EXCEPTION_INFORMATION  mei;
    mei.ThreadId = ::GetCurrentThreadId();
    mei.ExceptionPointers = exception_ptr;
    mei.ClientPointers = false;

    dump_writer(GetCurrentProcess(), GetCurrentProcessId(), handle, MiniDumpNormal, &mei, NULL, NULL);
    ::CloseHandle(handle);

    return EXCEPTION_EXECUTE_HANDLER;
}
#endif

void WindowsPlatformTraits::setup_crash_handler()
{
#if WITH_CRASH_HANDLER
    SetUnhandledExceptionFilter(exception_handler);
#endif
}

} // namespace atlas