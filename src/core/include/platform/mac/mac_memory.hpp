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

    static void Memcpy(void* dest, const void* src, size_t size)
    {
        std::memcpy(dest, src, size);
    }

    static void Memmove(void* dest, const void* src, size_t size)
    {
        std::memcpy(dest, src, size);
    }

    static void Memset(void* dest, byte value, size_t size)
    {
        std::memset(dest, value, size);
    }

    static bool Memcmp(void* left, void* right, size_t size)
    {
        return std::memcmp(left, right, size) == 0;
    }

    MacMemory() = delete;
};

typedef MacMemory PlatformMemory;

}