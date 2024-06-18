// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "file_system/directory.hpp"
#include "platform/platform_fwd.hpp"

namespace atlas
{

Path Directory::get_engine_directory()
{
    static Path directory = PlatformTraits::get_engine_directory().normalize();
    return directory;
}

Path Directory::get_engine_module_directory()
{
    return get_module_directory(get_engine_directory());
}

Path Directory::get_module_directory(const Path& search_path)
{
    return search_path / PlatformTraits::get_relative_build_directory();
}

Path Directory::get_engine_plugins_directory()
{
    Path engine_root = get_engine_directory();
    Path dir = engine_root / "plugins\\";
    return dir;
}

}
