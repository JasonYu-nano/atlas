// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "memory/memory.hpp"

#if PLATFORM_WINDOWS
#include "platform/windows/windows_memory.hpp"
#elif PLATFORM_APPLE
#include "platform/mac/mac_memory.hpp"
#elif PLATFORM_LINUX

#endif

namespace atlas
{

void* Memory::Malloc(size_t size)
{
    return GetMallocInstance()->Malloc(size);
}

void* Memory::AlignedMalloc(size_t size, size_t alignment)
{
    return GetMallocInstance()->AlignedMalloc(size, alignment);
}

void* Memory::Realloc(void* ptr, size_t new_size)
{
    return GetMallocInstance()->Realloc(ptr, new_size);
}

void* Memory::AlignedRealloc(void* ptr, size_t new_size, size_t alignment)
{
    return GetMallocInstance()->AlignedRealloc(ptr, new_size, alignment);
}

void Memory::Free(void* ptr)
{
    GetMallocInstance()->Free(ptr);
}

void Memory::AlignedFree(void* ptr)
{
    GetMallocInstance()->AlignedFree(ptr);
}

void Memory::Memcpy(void* dest, const void* src, size_t size)
{
    PlatformMemory::Memcpy(dest, src, size);
}

void Memory::Memmove(void* dest, const void* src, size_t size)
{
    PlatformMemory::Memmove(dest, src, size);
}

void Memory::Memset(void* dest, uint8 value, size_t size)
{
    PlatformMemory::Memset(dest, value, size);
}

bool Memory::Memcmp(void* left, void* right, size_t size)
{
    return PlatformMemory::Memcmp(left, right, size);
}

void Memory::MemmoveBits(uint32* dest, int32 dest_offset, uint32* src, int32 src_offset, uint32 bit_count)
{

}

MallocBase* Memory::GetMallocInstance()
{
    static std::unique_ptr<MallocBase> malloc_instance = PlatformMemory::GetDefaultMalloc();
    return malloc_instance.get();
}
}