// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "asset/package_loader.hpp"
#include "engine.hpp"
#include "gtest/gtest.h"

namespace atlas::test
{

TEST(AssetTest, ObjectPath)
{
    {
        ObjectPath path("engine:dir/package.object");
        EXPECT_EQ(path.get_owner_assembly(), "engine");
        EXPECT_EQ(path.get_package_prefix(), "dir/package");
        EXPECT_EQ(path.get_object_name(), "object");
    }
    {
        ObjectPath path("dir/package.");
        EXPECT_EQ(path.get_package_prefix(), "dir/package");
        EXPECT_TRUE(path.get_object_name().empty());
    }
    {
        ObjectPath path("engine:.object");
        EXPECT_TRUE(path.get_package_prefix().empty());
        EXPECT_EQ(path.get_object_name(), "object");
    }
    {
        ObjectPath path;
        EXPECT_EQ(path.get_package_prefix(), "");
        EXPECT_EQ(path.get_object_name(), "");
    }
    {
        ObjectPath path1("engine:dir/package.object");
        ObjectPath path2("engine:dir/package.object");
        ObjectPath path3("engine:dir/package.object2");
        EXPECT_EQ(path1, path2);
        EXPECT_NE(path1, path3);
    }
}

TEST(AssetTest, SaveAndLoad)
{
    AssetPackage package;
    AssetOverview overview{ GUID::new_guid(), {GUID::new_guid(), GUID::new_guid()}, 0, 0, ObjectPath() };
    package.assets_.insert(overview.id, overview);

    Path save_path = Directory::get_engine_save_directory() / "test_package.ap";

    static LowLevelIO llio;
    auto package_loader = std::make_unique<PackageLoader>(llio);
    package_loader->set_additional_save_to_txt(true);

    auto task = launch(package_loader->save_package(package, save_path));

    EXPECT_TRUE(task.get_result() == true);

    auto task2 = launch(package_loader->load_package(save_path));
    auto loaded_pkg = task2.get_result();

    EXPECT_EQ(loaded_pkg->assets_.size(), 1);
    EXPECT_EQ(loaded_pkg->assets_.begin()->first, overview.id);
}

}