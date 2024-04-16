// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <memory>

#include "memory/standard_malloc.hpp"

namespace atlas
{
class CORE_API MacMemory
{
public:
    static std::unique_ptr<MallocBase> GetDefaultMalloc()
    {
        return std::make_unique<StandardMalloc>();
    }

    MacMemory() = delete;
};

typedef MacMemory PlatformMemory;

}