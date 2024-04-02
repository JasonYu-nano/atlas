// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <memory>

#include "memory/malloc_base.hpp"

#if PLATFORM_WINDOWS
#include "platform/windows/windows_memory.hpp"
#elif PLATFORM_APPLE
#include "platform/mac/mac_memory.hpp"
#elif PLATFORM_LINUX
#endif

namespace atlas
{
class CORE_API Memory
{
public:
    static void* Malloc(size_t size)
    {
        return GetMallocInstance()->Malloc(size);
    }

    static void* AlignedMalloc(size_t size, size_t alignment)
    {
        return GetMallocInstance()->AlignedMalloc(size, alignment);
    }

    static void* Realloc(void* ptr, size_t new_size)
    {
        return GetMallocInstance()->Realloc(ptr, new_size);
    }

    static void* AlignedRealloc(void* ptr, size_t new_size, size_t alignment)
    {
        return GetMallocInstance()->AlignedRealloc(ptr, new_size, alignment);
    }

    static void Free(void* ptr)
    {
        GetMallocInstance()->Free(ptr);
    }

    static void AlignedFree(void* ptr)
    {
        GetMallocInstance()->AlignedFree(ptr);
    }

    static void Memcpy(void* dest, const void* src, size_t size)
    {
        PlatformMemory::Memcpy(dest, src, size);
    }

    static void Memmove(void* dest, const void* src, size_t size)
    {
        PlatformMemory::Memmove(dest, src, size);
    }

    static void Memset(void* dest, byte value, size_t size)
    {
        PlatformMemory::Memset(dest, value, size);
    }

    static bool Memcmp(void* left, void* right, size_t size)
    {
        return PlatformMemory::Memcmp(left, right, size);
    }

    static void MemmoveBits(uint32* dest, int32 dest_offset, uint32* src, int32 src_offset, uint32 bit_count);

    Memory() = delete;
    ~Memory() = delete;

private:
    static MallocBase* GetMallocInstance()
    {
        if (!malloc_instance_)
        {
            malloc_instance_ = PlatformMemory::GetDefaultMalloc();
        }

        return malloc_instance_.get();
    }

    static inline std::unique_ptr<MallocBase> malloc_instance_;
};
}