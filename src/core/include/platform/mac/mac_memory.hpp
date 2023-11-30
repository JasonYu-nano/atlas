// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <memory>

#include "memory/malloc_interface.hpp"

namespace atlas
{
class CORE_API MacMemory
{
public:
    static std::unique_ptr<IMalloc> GetDefaultMalloc();

    static void Memcpy(void* dest, const void* src, size_t size);

    static void Memmove(void* dest, const void* src, size_t size);

    static void Memset(void* dest, byte value, size_t size);

    static bool Memcmp(void* left, void* right, size_t size);

    static uint32 DefaultAlignment;

    MacMemory() = delete;
};

typedef MacMemory PlatformMemory;

}