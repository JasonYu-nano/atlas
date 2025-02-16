// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "asset/package_loader.hpp"

#include "file_system/directory.hpp"

namespace atlas
{

Path PackageLoader::get_package_path(const ObjectPath& path) const
{
    StringView assembly = path.get_owner_assembly();
    if (assembly.empty())
    {
        return {};
    }

    StringView pkg_relative = path.get_package_prefix();
    if (pkg_relative.empty())
    {
        return {};
    }

    if (assembly == "engine")
    {
        return Directory::get_engine_directory() / String(pkg_relative) / AssetPackage::package_extension;
    }
    if (assembly == "project")
    {
        return Directory::get_project_directory() / String(pkg_relative) / AssetPackage::package_extension;
    }

    //TODO: find plugin path
    return {};
}

Task<bool> PackageLoader::save_package(const AssetPackage& package, const Path& path)
{
    CompactBinaryArchiveWriter writer;

    if (additional_save_to_txt_)
    {
        auto new_path = path.replace_extension(".json");
        JsonArchiveWriter json_writer;
        co_await save_package_impl(json_writer, package, new_path);
    }

    bool ret = co_await save_package_impl(writer, package, path);
    co_return ret;
}

Task<std::shared_ptr<AssetPackage>> PackageLoader::load_package(const ObjectPath& path)
{
    auto pkg_path = get_package_path(path);
    return load_package(pkg_path);
}

Task<std::shared_ptr<AssetPackage>> PackageLoader::load_package(const Path& pkg_path)
{
    if (!pkg_path.exists())
    {
        co_return nullptr;
    }

    auto head_size_buffer = co_await llio_.async_read(pkg_path, AssetPackage::package_head_max_size_, EIOPriority::Normal);
    if (head_size_buffer.size() < AssetPackage::package_head_max_size_)
    {
        co_return nullptr;
    }

    CompactBinaryArchiveReader size_reader(head_size_buffer);
    FixedInt<AssetPackage::head_size_type> head_size = 0;
    size_reader >> head_size;
    size_t next_read_offset = size_reader.tell();

    auto head_buffer = co_await llio_.async_read(pkg_path, next_read_offset, head_size, EIOPriority::Normal);
    CompactBinaryArchiveReader head_reader(head_buffer);
    auto package = std::make_shared<AssetPackage>();
    head_reader >> *package;

    co_return package;
}

Task<bool> PackageLoader::save_package_impl(WriteStream& ws, const AssetPackage& package, const Path& path)
{
    AssetPackage::head_size_type head_size = 0;
    ws << FixedInt(head_size);
    auto pos = ws.tell();

    ws << package;
    head_size = ws.tell() - pos;

    {
        ScopeStreamSeek seek_to(ws, 0);
        ws << FixedInt(head_size);
    }

    //TODO: write asset data

    auto write_buffer = ws.get_buffer();
    size_t write_size = co_await llio_.async_write(path, write_buffer);

    co_return write_buffer.size() == write_size;
}

}// namespace atlas