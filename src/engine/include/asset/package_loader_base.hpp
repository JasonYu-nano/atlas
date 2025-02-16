// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "asset_package.hpp"
#include "async/task.hpp"
#include "file_system/path.hpp"
#include "object_path.hpp"

namespace atlas
{

class ENGINE_API PackageLoaderBase
{
public:
    virtual ~PackageLoaderBase() = default;

    NODISCARD virtual Path get_package_path(const ObjectPath& path) const
    {
        return {};
    }

    virtual Task<bool> save_package(const AssetPackage& package, const Path& path) = 0;

    virtual Task<std::shared_ptr<AssetPackage>> load_package(const ObjectPath& path) = 0;

    virtual Task<std::shared_ptr<AssetPackage>> load_package(const Path& pkg_path) = 0;
};

}// namespace atlas
