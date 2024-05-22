// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "platform/windows/windows_platform_traits.hpp"
#include "platform/windows/windows_minimal_api.hpp"
#include "string/string_name.hpp"
#include "core_log.hpp"

namespace atlas
{

void* WindowsPlatformTraits::LoadDynamicLibrary(const Path& path)
{
    auto&& sys_path = path.ToOSPath();
    void* handle = ::LoadLibrary(sys_path.data());
    if (handle == nullptr)
    {
        DWORD err_code = ::GetLastError();
        LOG_ERROR(core, "Load library {0} failed. error code: {1}", path.ToString(), err_code);
    }

    return handle;
}

void WindowsPlatformTraits::FreeDynamicLibrary(void* module_handle)
{
    ASSERT(module_handle);
    ::FreeLibrary((HMODULE)module_handle);
}

void* WindowsPlatformTraits::GetExportedSymbol(void* handle, const String& symbol_name)
{
    return ::GetProcAddress((HMODULE)handle, symbol_name.Data());
}

const Path& WindowsPlatformTraits::GetEngineDirectory()
{
    static Path dir = GetEngineDirectoryImpl();
    return dir;
}

const Path& WindowsPlatformTraits::GetRelativeBuildDirectory()
{
#if DEBUG
    static Path reltive_path("build\\debug\\out\\bin");
#elif DEBUG_OPTIMIZE
    static Path reltive_path("build\\debug_optimize\\out\\bin");
#elif RELEASE
    static Path reltive_path("build\\release\\out\\bin");
#else
#   error "Invalid build type"
#endif
    return reltive_path;
}

Path WindowsPlatformTraits::GetDynamicLibraryPath(const Path& module_dir, const StringName& lib_name)
{
#if DEBUG
    return module_dir / String::Format("{0}d.dll", lib_name.ToLexical());
#else
    return module_dir / lib_name.ToLexical();
#endif
}

Path WindowsPlatformTraits::GetEngineDirectoryImpl()
{
    std::error_code error;
    std::filesystem::path working_directory = std::filesystem::current_path(error);
    String working_directory_string = String::From(working_directory);
    working_directory_string.Remove(GetRelativeBuildDirectory().ToString());
    return { working_directory_string };
}

} // namespace atlas
