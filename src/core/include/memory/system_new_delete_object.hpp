// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <new>

#include "core_def.hpp"

namespace atlas
{
/** inherit from SystemNewDeleteObject to avoid use override operator new delete */
class CORE_API SystemNewDeleteObject
{
public:
    void* operator new(size_t size);

    void* operator new[](size_t size);

    void* operator new(size_t size, std::align_val_t alignment);

    void* operator new[](size_t size, std::align_val_t alignment);

    void* operator new(size_t size, const std::nothrow_t& tag) noexcept;

    void* operator new[](size_t size, const std::nothrow_t& tag) noexcept;

    void* operator new(size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept;

    void* operator new[](size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept;

    void operator delete(void* ptr) noexcept;

    void operator delete[](void* ptr) noexcept;

    void operator delete  (void* ptr, std::align_val_t alignment) noexcept;

    void operator delete[](void* ptr, std::align_val_t alignment) noexcept;

    void operator delete  (void* ptr, size_t size) noexcept;

    void operator delete[](void* ptr, size_t size) noexcept;

    void operator delete  (void* ptr, size_t size, std::align_val_t alignment) noexcept;

    void operator delete[](void* ptr, size_t size, std::align_val_t alignment) noexcept;
};
}