// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "io/llio.hpp"
#include "package_loader_base.hpp"
#include "serialize/compact_binary_archive.hpp"
#include "serialize/json_archive.hpp"

namespace atlas
{

class ENGINE_API PackageLoader : public PackageLoaderBase
{
public:
    explicit PackageLoader(LowLevelIO& llio) : llio_(llio) {}

    ~PackageLoader() override = default;

    void set_additional_save_to_txt(bool state) { additional_save_to_txt_ = state; }

    NODISCARD Path get_package_path(const ObjectPath& path) const override;

    Task<bool> save_package(const AssetPackage& package, const Path& path) override;

    Task<std::shared_ptr<AssetPackage>> load_package(const ObjectPath& path) override;

    Task<std::shared_ptr<AssetPackage>> load_package(const Path& pkg_path) override;

protected:
    Task<bool> save_package_impl(WriteStream& ws, const AssetPackage& package, const Path& path);

    LowLevelIO& llio_;
    bool additional_save_to_txt_{ false };
};

}// namespace atlas
