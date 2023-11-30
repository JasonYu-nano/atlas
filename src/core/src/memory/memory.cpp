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

std::unique_ptr<IMalloc> Memory::malloc_instance_ = PlatformMemory::GetDefaultMalloc();

void* Memory::Malloc(size_t size)
{
    return malloc_instance_->Malloc(size);
}

void* Memory::AlignedMalloc(size_t size, uint32 alignment)
{
    return malloc_instance_->AlignedMalloc(size, alignment);
}

void* Memory::Realloc(void* ptr, size_t new_size)
{
    return malloc_instance_->Realloc(ptr, new_size);
}

void* Memory::AlignedRealloc(void* ptr, size_t new_size, uint32 alignment)
{
    return malloc_instance_->AlignedRealloc(ptr, new_size, alignment);
}

void Memory::Free(void* ptr)
{
    malloc_instance_->Free(ptr);
}

void Memory::AlignedFree(void* ptr)
{
    malloc_instance_->AlignedFree(ptr);
}

void Memory::Memcpy(void* dest, const void* src, size_t size)
{
    PlatformMemory::Memcpy(dest, src, size);
}

void Memory::Memmove(void* dest, void* src, size_t size)
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
}