// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <new>

#include "core_def.hpp"


CORE_API void* operator new  (size_t size);
CORE_API void* operator new[](size_t size);
CORE_API void* operator new  (size_t size, std::align_val_t alignment);
CORE_API void* operator new[](size_t size, std::align_val_t alignment);
CORE_API void* operator new  (size_t size, const std::nothrow_t& tag) noexcept;
CORE_API void* operator new[](size_t size, const std::nothrow_t& tag) noexcept;
CORE_API void* operator new  (size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept;
CORE_API void* operator new[](size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept;


CORE_API void operator delete  (void* ptr) noexcept;
CORE_API void operator delete[](void* ptr) noexcept;
CORE_API void operator delete  (void* ptr, std::align_val_t alignment) noexcept;
CORE_API void operator delete[](void* ptr, std::align_val_t alignment) noexcept;
CORE_API void operator delete  (void* ptr, size_t size) noexcept;
CORE_API void operator delete[](void* ptr, size_t size) noexcept;
CORE_API void operator delete  (void* ptr, size_t size, std::align_val_t alignment) noexcept;
CORE_API void operator delete[](void* ptr, size_t size, std::align_val_t alignment) noexcept;