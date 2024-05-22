// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "file_system/directory.hpp"
#include "platform/platform_fwd.hpp"

namespace atlas
{

Path Directory::GetEngineDirectory()
{
    static Path directory = PlatformTraits::GetEngineDirectory().Normalize();
    return directory;
}

Path Directory::GetEngineModuleDirectory()
{
    return GetModuleDirectory(GetEngineDirectory());
}

Path Directory::GetModuleDirectory(const Path& search_path)
{
    return search_path / PlatformTraits::GetRelativeBuildDirectory();
}

Path Directory::GetEnginePluginsDirectory()
{
    Path engine_root = GetEngineDirectory();
    Path dir = engine_root / "plugins\\";
    return dir;
}

}
