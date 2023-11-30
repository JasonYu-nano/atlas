// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <memory>

#include "memory/malloc_interface.hpp"

namespace atlas
{
class CORE_API Memory
{
public:
    static void* Malloc(size_t size);

    static void* AlignedMalloc(size_t size, uint32 alignment);

    static void* Realloc(void* ptr, size_t new_size);

    static void* AlignedRealloc(void* ptr, size_t new_size, uint32 alignment);

    static void Free(void* ptr);

    static void AlignedFree(void* ptr);

    static void Memcpy(void* dest, void const* src, size_t size);

    static void Memmove(void* dest, void* src, size_t size);

    static void Memset(void* dest, byte value, size_t size);

    static bool Memcmp(void* left, void* right, size_t size);

    static void MemmoveBits(uint32* dest, int32 dest_offset, uint32* src, int32 src_offset, uint32 bit_count);

    Memory() = delete;
    ~Memory() = delete;

private:
    static std::unique_ptr<IMalloc> malloc_instance_;
};
}