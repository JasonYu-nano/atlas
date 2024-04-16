// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <memory>

#include "memory/malloc_base.hpp"
#include "memory/standard_malloc.hpp"

namespace atlas
{
    class CORE_API WindowsMemory
    {
    public:
        static std::unique_ptr<MallocBase> GetDefaultMalloc()
        {
            return std::make_unique<StandardMalloc>() ;
        }

        WindowsMemory() = delete;
    };

    typedef WindowsMemory PlatformMemory;

}