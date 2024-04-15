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

Path Directory::GetModuleDirectory()
{
    Path engine_root = GetEngineDirectory();
    Path dir = engine_root / "build\\debug\\out\\bin\\";
    return dir;
}

Path Directory::GetEnginePluginsDirectory()
{
    Path engine_root = GetEngineDirectory();
    Path dir = engine_root / "plugins\\";
    return dir;
}

}
