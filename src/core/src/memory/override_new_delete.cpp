// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "memory/override_new_delete.hpp"
#include "memory/memory.hpp"

void* operator new(size_t size)
{
    if (size == 0)
    {
        ++size; // avoid malloc(0) which may return nullptr on success
    }

    if (void* ptr = atlas::Memory::malloc(size))
    {
        return ptr;
    }

#if CPP_EXCEPTIONS
    throw std::bad_alloc{};
#else
    std::unreachable();
#endif
}

void* operator new[](size_t size)
{
    if (size == 0)
    {
        ++size; // avoid malloc(0) which may return nullptr on success
    }

    if (void* ptr = atlas::Memory::malloc(size))
    {
        return ptr;
    }

#if CPP_EXCEPTIONS
    throw std::bad_alloc{};
#else
    std::unreachable();
#endif
}

void* operator new(size_t size, std::align_val_t alignment)
{
    if (size == 0)
    {
        ++size; // avoid malloc(0) which may return nullptr on success
    }

    if (void* ptr = atlas::Memory::aligned_malloc(size, static_cast<size_t>(alignment)))
    {
        return ptr;
    }

#if CPP_EXCEPTIONS
    throw std::bad_alloc{};
#else
    std::unreachable();
#endif
}

void* operator new[](size_t size, std::align_val_t alignment)
{
    if (size == 0)
    {
        ++size; // avoid malloc(0) which may return nullptr on success
    }

    if (void* ptr = atlas::Memory::aligned_malloc(size, static_cast<size_t>(alignment)))
    {
        return ptr;
    }

#if CPP_EXCEPTIONS
    throw std::bad_alloc{};
#else
    std::unreachable();
#endif
}

void* operator new(size_t size, const std::nothrow_t &tag) noexcept
{
    if (size == 0)
    {
        ++size; // avoid malloc(0) which may return nullptr on success
    }

    return atlas::Memory::malloc(size);
}
void* operator new[](size_t size, const std::nothrow_t &tag) noexcept
{
    if (size == 0)
    {
        ++size; // avoid malloc(0) which may return nullptr on success
    }

    return atlas::Memory::malloc(size);
}

void* operator new(size_t size, std::align_val_t alignment, const std::nothrow_t &) noexcept
{
    if (size == 0)
    {
        ++size; // avoid malloc(0) which may return nullptr on success
    }

    return atlas::Memory::aligned_malloc(size, static_cast<size_t>(alignment));
}

void* operator new[](size_t size, std::align_val_t alignment, const std::nothrow_t &) noexcept
{
    if (size == 0)
    {
        ++size; // avoid malloc(0) which may return nullptr on success
    }

    return atlas::Memory::aligned_malloc(size, static_cast<size_t>(alignment));
}

void operator delete(void* ptr) noexcept
{
    atlas::Memory::free(ptr);
}

void operator delete[](void* ptr) noexcept
{
    atlas::Memory::free(ptr);
}

void operator delete(void* ptr, std::align_val_t alignment) noexcept
{
    atlas::Memory::aligned_free(ptr);
}

void operator delete[](void* ptr, std::align_val_t alignment) noexcept
{
    atlas::Memory::aligned_free(ptr);
}

void operator delete(void* ptr, size_t size) noexcept
{
    atlas::Memory::free(ptr);
}

void operator delete[](void* ptr, size_t size) noexcept
{
    atlas::Memory::free(ptr);
}

void operator delete(void* ptr, size_t size, std::align_val_t alignment) noexcept
{
    atlas::Memory::aligned_free(ptr);
}

void operator delete[](void* ptr, size_t size, std::align_val_t alignment) noexcept
{
    atlas::Memory::aligned_free(ptr);
}
