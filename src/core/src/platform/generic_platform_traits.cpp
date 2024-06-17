// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "platform/generic_platform_traits.hpp"

namespace atlas
{

static Path get_engine_directory_impl()
{
    std::error_code error;
    std::filesystem::path working_directory = std::filesystem::current_path(error);
    String working_directory_string = String::from(working_directory);
    working_directory_string.remove(GenericPlatformTraits::get_relative_build_directory().to_string());
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
    static Path relative_path = Path(R"(build\debug\out\bin)").normalize();
#elif DEBUG_OPTIMIZE
    static Path relative_path = Path(R"(build\debug_optimize\out\bin)").normalize();
#elif RELEASE
    static Path relative_path = Path(R"(build\release\out\bin)").normalize();
#else
#   error "Invalid build type"
#endif
    return relative_path;
}

Path GenericPlatformTraits::get_library_path(const Path& module_dir, StringName lib_name)
{
#if DEBUG
    return module_dir / String::format("{0}d.dll", lib_name.to_lexical());
#else
    return module_dir / lib_name.to_lexical();
#endif
}

}// namespace atlas