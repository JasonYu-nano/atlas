// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "core_def.hpp"
#include "object_path.hpp"
#include "utility/guid.hpp"
#include "asset.gen.hpp"

namespace atlas
{

struct ENGINE_API AssetOverview
{
    GUID id;
    Array<GUID> import_assets;
    size_t offset;
    size_t size;
    ObjectPath path;

    friend void serialize(WriteStream& ws, const AssetOverview& v)
    {
        ws << v.id;
        ws << v.import_assets.size();
        for (auto&& imported : v.import_assets)
        {
            ws << imported;
        }
        ws << v.offset;
        ws << v.size;
    }

    friend void deserialize(ReadStream& rs, AssetOverview& v)
    {
        rs >> v.id;
        size_t size = 0;
        rs >> size;
        for (size_t i = 0; i < size; ++i)
        {
            GUID imported;
            rs >> imported;
            v.import_assets.add(std::move(imported));
        }

        rs >> v.offset;
        rs >> v.size;
    }
};

class ENGINE_API META() Asset
{
    GEN_META_CODE(Asset)
public:
    const AssetOverview& get_overview() const { return overview_; }

    const IOBuffer& get_buffer() const { return buffer_; }

private:
    AssetOverview overview_;
    IOBuffer buffer_;
};

}// namespace atlas