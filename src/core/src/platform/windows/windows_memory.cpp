// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "platform/windows/windows_memory.hpp"

#include <cstring>

#include "memory/standard_malloc.hpp"

namespace atlas
{
std::unique_ptr<MallocBase> WindowsMemory::GetDefaultMalloc()
{
    return std::make_unique<StandardMalloc>() ;
}

void WindowsMemory::Memcpy(void* dest, const void* src, size_t size)
{
    ::memcpy(dest, src, size);
}

void WindowsMemory::Memmove(void* dest, const void* src, size_t size)
{
    ::memcpy(dest, src, size);
}

void WindowsMemory::Memset(void* dest, byte value, size_t size)
{
    ::memset(dest, value, size);
}

bool WindowsMemory::Memcmp(void* left, void* right, size_t size)
{
    return ::memcmp(left, right, size) == 0;
}
}