// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "gtest/gtest.h"

#include "file_system/directory.hpp"

namespace atlas::test
{

TEST(FileSystemTest, PathTest)
{
#if PLATFORM_WINDOWS
    {
        Path path("c:\\user");
        EXPECT_TRUE(path / "atlas" == Path("c:\\user\\atlas"));
        EXPECT_TRUE(path + "\\atlas" == Path("c:\\user\\atlas"));
    }
    {
        Path path("c:/user/atlas");
        EXPECT_TRUE(path.MakePreferred() == Path("c:\\user\\atlas"));
    }
    {
        Path path("c:\\user/atlas/..////game/./");
        Path npath = path.Normalize();
        EXPECT_TRUE(npath == Path("c:\\user\\game\\"));
    }
#else
    {
        Path path("user");
        EXPECT_TRUE(path / "atlas" == Path("user/atlas"));
        EXPECT_TRUE(path + "/atlas" == Path("user/atlas"));
    }
    {
        Path path("\\user\\atlas");
        EXPECT_TRUE(path.MakePreferred() == Path("/user/atlas"));
    }
    {
        Path path("\\user/atlas/..////game/./");
        Path npath = path.Normalize();
        EXPECT_TRUE(npath == Path("/user/game/"));
    }
#endif
}

} // namespace atlas::test
