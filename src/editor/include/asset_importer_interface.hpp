// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "async/task.hpp"
#include "file_system/path.hpp"

namespace atlas
{

class IAssetImporter
{
public:
    virtual ~IAssetImporter() = default;

    virtual Array<String> get_associated_extensions() = 0;

    virtual Task<> import(const Path& file) = 0;
};

}// namespace atlas