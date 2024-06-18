// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "platform/generic_platform_traits.hpp"

namespace atlas
{

static Path get_engine_directory_impl()
{
    std::error_code error;
    std::filesystem::path working_directory = std::filesystem::current_path(error);
    String working_directory_string = String::From(working_directory);
    working_directory_string.Remove(GenericPlatformTraits::get_relative_build_directory().to_string());
    return { working_directory_string };
}

const Path& GenericPlatformTraits::get_engine_directory()
{
    static Path dir = get_engine_directory_impl();
    return dir;
}

const Path& GenericPlatformTraits::get_relative_build_directory()
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

Path GenericPlatformTraits::get_library_path(const Path& module_dir, StringName lib_name)
{
#if DEBUG
    return module_dir / String::Format("{0}d.dll", lib_name.ToLexical());
#else
    return module_dir / lib_name.ToLexical();
#endif
}

}// namespace atlas