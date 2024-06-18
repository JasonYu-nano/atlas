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

    virtual void* malloc(size_t size) = 0;

    virtual void* aligned_malloc(size_t size, size_t alignment) = 0;

    virtual void* realloc(void* ptr, size_t new_size) = 0;

    virtual void* aligned_realloc(void* ptr, size_t new_size, size_t alignment) = 0;

    virtual void free(void* ptr) = 0;

    virtual void aligned_free(void* ptr) = 0;
};
}