// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "asset.hpp"

namespace atlas
{

class ENGINE_API AssetPackage
{
public:
    using head_size_type = uint32;
    static constexpr uint32 package_head_max_size_ = sizeof(head_size_type);
    inline static const char* package_extension = ".ap";

    friend void serialize(WriteStream& ws, const AssetPackage& v)
    {
        ws << v.assets_.size();
        for (auto&& pair : v.assets_)
        {
            ws << pair.second;
        }
    }

    friend void deserialize(ReadStream& rs, AssetPackage& v)
    {
        size_t map_size = 0;
        rs >> map_size;
        for (size_t i = 0; i < map_size; ++i)
        {
            AssetOverview overview;
            rs >> overview;
            v.assets_.insert(overview.id, overview);
        }
    }

    const AssetOverview* get_overview(const ObjectPath& path) const
    {
        for (auto&& pair : assets_)
        {
            if (pair.second.path == path)
            {
                return &(pair.second);
            }
        }
        return nullptr;
    }

// private:
    UnorderedMap<GUID, AssetOverview> assets_;
};

}// namespace atlas
