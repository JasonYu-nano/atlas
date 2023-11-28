// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "platform/mac/mac_memory.hpp"

#include <cstring>

#include "memory/standard_malloc.hpp"

namespace atlas
{
IMalloc* MacMemory::GetDefaultMalloc()
{
    return new StandardMalloc();
}

void MacMemory::DestroyMalloc(IMalloc* malloc)
{
    delete malloc;
}

void MacMemory::Memcpy(void* dest, const void* src, size_t size)
{
    ::memcpy(dest, src, size);
}

void MacMemory::Memmove(void* dest, const void* src, size_t size)
{
    ::memcpy(dest, src, size);
}

void MacMemory::Memset(void* dest, byte value, size_t size)
{
    ::memset(dest, value, size);
}

bool MacMemory::Memcmp(void* left, void* right, size_t size)
{
    return ::memcmp(left, right, size) == 0;
}
}