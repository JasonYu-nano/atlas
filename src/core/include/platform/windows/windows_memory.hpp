// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <memory>

#include "memory/malloc_base.hpp"

namespace atlas
{
    class CORE_API WindowsMemory
    {
    public:
        static std::unique_ptr<MallocBase> GetDefaultMalloc();

        static void Memcpy(void* dest, const void* src, size_t size);

        static void Memmove(void* dest, const void* src, size_t size);

        static void Memset(void* dest, byte value, size_t size);

        static bool Memcmp(void* left, void* right, size_t size);

        WindowsMemory() = delete;
    };

    typedef WindowsMemory PlatformMemory;

}