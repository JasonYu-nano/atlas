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
    static void* malloc(size_t size)
    {
        return get_malloc_instance()->malloc(size);
    }

    static void* aligned_malloc(size_t size, size_t alignment)
    {
        return get_malloc_instance()->aligned_malloc(size, alignment);
    }

    static void* realloc(void* ptr, size_t new_size)
    {
        return get_malloc_instance()->realloc(ptr, new_size);
    }

    static void* aligned_realloc(void* ptr, size_t new_size, size_t alignment)
    {
        return get_malloc_instance()->aligned_realloc(ptr, new_size, alignment);
    }

    static void free(void* ptr)
    {
        get_malloc_instance()->free(ptr);
    }

    static void aligned_free(void* ptr)
    {
        get_malloc_instance()->aligned_free(ptr);
    }

    Memory() = delete;
    ~Memory() = delete;

private:
    static MallocBase* get_malloc_instance()
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