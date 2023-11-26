//  Copyright(c) 2023-present, Atlas.
//  Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "memory/c_malloc.hpp"

#include <cstdlib>

#include "memory/memory.hpp"
#include "math/atlas_math.hpp"

namespace atlas
{

void* CMalloc::Malloc(size_t size, uint32 alignment)
{
    // additional records of allocated memory size and original address.
    void* ptr = ::malloc(size + alignment + sizeof(void*) + sizeof(size_t));
    void* result;
    result = math::Align((uint8*)ptr + sizeof(void*) + sizeof(size_t), alignment);
    *((void**)((uint8*)result - sizeof(void*))) = ptr;
    *((size_t*)((uint8*)result - sizeof(void*) - sizeof(size_t))) = size;
    return result;
}

void CMalloc::Free(void* ptr)
{
    ::free(*((void**)((uint8*)ptr - sizeof(void*))));
}

void* CMalloc::Realloc(void* ptr, size_t new_size, uint32 alignment)
{
    alignment = math::Max(new_size >= 16 ? (uint32)16 : (uint32)8, alignment);

    void* new_ptr = Malloc(new_size, alignment);
    size_t size = GetAllocateSize(ptr);
    Memory::Memcpy(new_ptr, ptr, math::Min(new_size, size));
    Free(ptr);
    return new_ptr;
}

size_t CMalloc::GetAllocateSize(void* ptr)
{
    return *((size_t*)((uint8*)ptr - sizeof(void*) - sizeof(size_t)));
}
}