// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "core_def.hpp"
#include "memory/memory.hpp"

namespace atlas
{

class CORE_API GOAllocator
{
public:
    void* allocate(size_t size, size_t alignment)
    {
        return Memory::aligned_malloc(size, alignment);
    }

    void deallocate(void* ptr)
    {
        if (nullptr != ptr)
        {
            Memory::aligned_free(ptr);
        }
    }
};

}// namespace atlas