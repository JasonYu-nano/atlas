// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "core_def.hpp"

namespace atlas
{
class IMalloc
{
public:
    virtual ~IMalloc() = default;

    virtual void* Malloc(size_t size) = 0;

    virtual void* AlignedMalloc(size_t size, uint32 alignment) = 0;

    virtual void* Realloc(void* ptr, size_t new_size) = 0;

    virtual void* AlignedRealloc(void* ptr, size_t new_size, uint32 alignment) = 0;

    virtual void Free(void* ptr) = 0;

    virtual void AlignedFree(void* ptr) = 0;
};
}