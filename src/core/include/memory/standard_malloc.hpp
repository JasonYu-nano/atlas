// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "memory/malloc_base.hpp"

namespace atlas
{
class CORE_API StandardMalloc : public MallocBase
{
public:
    StandardMalloc() = default;

    ~StandardMalloc() override = default;

    void* Malloc(size_t size) override;

    void* AlignedMalloc(size_t size, size_t alignment) override;

    void* Realloc(void* ptr, size_t new_size) override;

    void* AlignedRealloc(void *ptr, size_t new_size, size_t alignment) override;

    void Free(void* ptr) override;

    void AlignedFree(void *ptr) override;
};

}