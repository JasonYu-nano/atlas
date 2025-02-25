// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <cstdlib>
#include <utility>

#include "memory/system_new_delete_object.hpp"

namespace atlas
{

void* SystemNewDeleteObject::operator new(size_t size)
{
    if (void* ptr = std::malloc(size))
    {
        return ptr;
    }

#if CPP_EXCEPTIONS
    throw std::bad_alloc{};
#else
    std::unreachable();
#endif
}

void* SystemNewDeleteObject::operator new[](size_t size)
{
    if (void* ptr = std::malloc(size))
    {
        return ptr;
    }

#if CPP_EXCEPTIONS
    throw std::bad_alloc{};
#else
    std::unreachable();
#endif
}

void* SystemNewDeleteObject::operator new(size_t size, std::align_val_t alignment)
{
#if PLATFORM_WINDOWS
    if (void* ptr = ::_aligned_malloc(size, static_cast<size_t>(alignment)))
#else
    if (void* ptr = std::aligned_alloc(static_cast<size_t>(alignment), size))
#endif
    {
        return ptr;
    }

#if CPP_EXCEPTIONS
    throw std::bad_alloc{};
#else
    std::unreachable();
#endif
}

void* SystemNewDeleteObject::operator new[](size_t size, std::align_val_t alignment)
{
#if PLATFORM_WINDOWS
    if (void* ptr = ::_aligned_malloc(size, static_cast<size_t>(alignment)))
#else
    if (void* ptr = std::aligned_alloc(static_cast<size_t>(alignment), size))
#endif
    {
        return ptr;
    }

#if CPP_EXCEPTIONS
    throw std::bad_alloc{};
#else
    std::unreachable();
#endif
}

void* SystemNewDeleteObject::operator new(size_t size, const std::nothrow_t &tag) noexcept
{
    return std::malloc(size);
}

void* SystemNewDeleteObject::operator new[](size_t size, const std::nothrow_t &tag) noexcept
{
    return std::malloc(size);
}

void* SystemNewDeleteObject::operator new(size_t size, std::align_val_t alignment, const std::nothrow_t &) noexcept
{
#if PLATFORM_WINDOWS
    return ::_aligned_malloc(size, static_cast<size_t>(alignment));
#else
    return std::aligned_alloc(static_cast<size_t>(alignment), size);
#endif
}

void* SystemNewDeleteObject::operator new[](size_t size, std::align_val_t alignment, const std::nothrow_t &) noexcept
{
#if PLATFORM_WINDOWS
    return ::_aligned_malloc(size, static_cast<size_t>(alignment));
#else
    return std::aligned_alloc(static_cast<size_t>(alignment), size);
#endif
}

void SystemNewDeleteObject::operator delete(void* ptr) noexcept
{
    std::free(ptr);
}

void SystemNewDeleteObject::operator delete[](void* ptr) noexcept
{
    std::free(ptr);
}

void SystemNewDeleteObject::operator delete(void* ptr, std::align_val_t alignment) noexcept
{
#if PLATFORM_WINDOWS
    return ::_aligned_free(ptr);
#else
    return std::free(ptr);
#endif
}

void SystemNewDeleteObject::operator delete[](void* ptr, std::align_val_t alignment) noexcept
{
#if PLATFORM_WINDOWS
    return ::_aligned_free(ptr);
#else
    return std::free(ptr);
#endif
}

void SystemNewDeleteObject::operator delete(void* ptr, size_t size) noexcept
{
    std::free(ptr);
}

void SystemNewDeleteObject::operator delete[](void* ptr, size_t size) noexcept
{
    std::free(ptr);
}

void SystemNewDeleteObject::operator delete(void* ptr, size_t size, std::align_val_t alignment) noexcept
{
#if PLATFORM_WINDOWS
    return ::_aligned_free(ptr);
#else
    return std::free(ptr);
#endif
}

void SystemNewDeleteObject::operator delete[](void* ptr, size_t size, std::align_val_t alignment) noexcept
{
#if PLATFORM_WINDOWS
    return ::_aligned_free(ptr);
#else
    return std::free(ptr);
#endif
}
}
