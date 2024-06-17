// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <cstdlib>

#include "memory/malloc_base.hpp"
#include "assertion.hpp"

#if PLATFORM_APPLE
#include <malloc/malloc.h>
#endif

namespace atlas
{
class CORE_API StandardMalloc : public MallocBase
{
public:
    StandardMalloc() = default;

    ~StandardMalloc() override = default;

    void* malloc(size_t size) override
    {
        return std::malloc(size);
    }

    void* aligned_malloc(size_t size, size_t alignment) override
    {
#if PLATFORM_WINDOWS
        return ::_aligned_malloc(size, alignment);
#elif PLATFORM_APPLE
        ASSERT(size % alignment == 0);
        return std::aligned_alloc(alignment, size);
#else
        return std::aligned_alloc(alignment, size);
#endif
    }

    void* realloc(void* ptr, size_t new_size) override
    {
        return std::realloc(ptr, new_size);
    }

    void* aligned_realloc(void *ptr, size_t new_size, size_t alignment) override
    {
#if PLATFORM_WINDOWS
        return ::_aligned_realloc(ptr, new_size, alignment);
#elif PLATFORM_APPLE
        ASSERT(new_size % alignment == 0);

        if (ptr == nullptr)
        {
            // if ptr is nullptr, treat it as malloc
            return aligned_malloc(new_size, alignment);
        }

        if (new_size == 0)
        {
            // if new size is 0, treat it as free
            aligned_free(ptr);
            return nullptr;
        }

        // allocate a new block with the desired alignment
        void* new_ptr;
        new_ptr = aligned_malloc(new_size, alignment);
        if (new_ptr == nullptr)
        {
            // allocation failed
            return nullptr;
        }

        // copy data from the old block to the new block
        size_t copy_size = new_size <= ::malloc_size(ptr) ? new_size : ::malloc_size(ptr);
        std::memcpy(new_ptr, ptr, copy_size);

        // free the old block
        aligned_free(ptr);

        return new_ptr;
#endif
    }

    void free(void* ptr) override
    {
        std::free(ptr);
    }

    void aligned_free(void *ptr) override
    {
#if PLATFORM_WINDOWS
        ::_aligned_free(ptr);
#else
        return std::free(ptr);
#endif
    }
};

}