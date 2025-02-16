// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "asset/asset.hpp"
#include "asset/package_loader_base.hpp"
#include "container/unordered_map.hpp"

namespace atlas
{

class ObjectPath;

class ENGINE_API AssetManager
{
public:
    AssetManager();

    void sync_load(const ObjectPath& path)
    {
        // Step 1: convert object path to package path
        // Step 2: load package
    }
// private:
    std::unique_ptr<PackageLoaderBase> package_loader_{ nullptr };
    UnorderedMap<GUID, AssetOverview> asset_cache_;
};

}// namespace atlas
