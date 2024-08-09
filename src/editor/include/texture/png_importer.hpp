// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "asset_importer_interface.hpp"

namespace atlas
{

class PngImporter : public IAssetImporter
{
public:
    ~PngImporter() override = default;

    Array<String> get_associated_extensions() override
    {
        return {".png"};
    }

    Task<> import(const Path& file) override;
};

}// namespace atlas
