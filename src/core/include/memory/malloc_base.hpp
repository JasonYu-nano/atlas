// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <new>

#include "memory/system_new_delete_object.hpp"

namespace atlas
{
class MallocBase : public SystemNewDeleteObject
{
public:
    virtual ~MallocBase() = default;

    virtual void* Malloc(size_t size) = 0;

    virtual void* AlignedMalloc(size_t size, size_t alignment) = 0;

    virtual void* Realloc(void* ptr, size_t new_size) = 0;

    virtual void* AlignedRealloc(void* ptr, size_t new_size, size_t alignment) = 0;

    virtual void Free(void* ptr) = 0;

    virtual void AlignedFree(void* ptr) = 0;
};
}