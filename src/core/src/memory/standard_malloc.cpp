// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "memory/standard_malloc.hpp"

#include <cstdlib>

#include "memory/memory.hpp"
#include "assertion.hpp"

#if PLATFORM_APPLE
#include <malloc/malloc.h>
#endif

namespace atlas
{

void* StandardMalloc::Malloc(size_t size)
{
    return std::malloc(size);
}

void* StandardMalloc::AlignedMalloc(size_t size, size_t alignment)
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

void* StandardMalloc::Realloc(void* ptr, size_t new_size)
{
    return std::realloc(ptr, new_size);
}

void* StandardMalloc::AlignedRealloc(void* ptr, size_t new_size, size_t alignment)
{
#if PLATFORM_WINDOWS
    return ::_aligned_realloc(ptr, new_size, alignment);
#elif PLATFORM_APPLE
    ASSERT(new_size % alignment == 0);

    if (ptr == nullptr)
    {
        // if ptr is nullptr, treat it as malloc
        return AlignedMalloc(new_size, alignment);
    }

    if (new_size == 0)
    {
        // if new size is 0, treat it as free
        AlignedFree(ptr);
        return nullptr;
    }

    // allocate a new block with the desired alignment
    void* new_ptr;
    new_ptr = AlignedMalloc(new_size, alignment);
    if (new_ptr == nullptr)
    {
        // allocation failed
        return nullptr;
    }

    // copy data from the old block to the new block
    size_t copy_size = new_size <= ::malloc_size(ptr) ? new_size : ::malloc_size(ptr);
    Memory::Memcpy(new_ptr, ptr, copy_size);

    // free the old block
    AlignedFree(ptr);

    return new_ptr;
#endif
}

void StandardMalloc::Free(void* ptr)
{
    std::free(ptr);
}

void StandardMalloc::AlignedFree(void* ptr)
{
#if PLATFORM_WINDOWS
    ::_aligned_free(ptr);
#else
    return std::free(ptr);
#endif
}
}