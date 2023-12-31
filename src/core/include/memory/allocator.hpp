// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "assertion.hpp"
#include "memory/memory.hpp"
#include "utility/untyped_data.hpp"

namespace atlas
{

namespace details
{

template<size_t Size>
size_t constexpr GetByteSize(const size_t count)
{
    constexpr size_t max_size = size_t(-1) / Size;
    ASSERT(count <= max_size);
    return Size * count;
}

}

template<typename SizeTy>
struct StandardAllocator
{
    template <typename T>
    class Allocator
    {
    public:
        using value_type = T;
        using size_type = SizeTy;
        using difference_type = size_type;
        using propagate_on_container_move_assignment = std::true_type;

        Allocator() = default;
        Allocator(const Allocator&) noexcept {}
        template<typename Other>
        Allocator(const Allocator<Other>&) noexcept {}
        ~Allocator() = default;

        NODISCARD value_type* allocate(const size_type size)
        {
            return static_cast<value_type*>(allocate_impl<alignof(T)>(details::GetByteSize<sizeof(T)>(size)));
        }

        void deallocate(value_type* const ptr, const size_type size)
        {
            deallocate_impl<alignof(T)>(ptr);
        }

    private:
        template <size_t Align, std::enable_if_t<(Align <= __STDCPP_DEFAULT_NEW_ALIGNMENT__), int> = 0>
        void* allocate_impl(size_t byte_size)
        {
            return Memory::Malloc(byte_size);
        }

        template <size_t Align, std::enable_if_t<(Align > __STDCPP_DEFAULT_NEW_ALIGNMENT__), int> = 0>
        void* allocate_impl(size_t byte_size)
        {
            return Memory::AlignedMalloc(byte_size, Align);
        }

        template <size_t Align, std::enable_if_t<(Align <= __STDCPP_DEFAULT_NEW_ALIGNMENT__), int> = 0>
        void deallocate_impl(void* ptr)
        {
            Memory::Free(ptr);
        }

        template <size_t Align, std::enable_if_t<(Align > __STDCPP_DEFAULT_NEW_ALIGNMENT__), int> = 0>
        void deallocate_impl(void* ptr)
        {
            Memory::AlignedFree(ptr);
        }
    };

    StandardAllocator() = delete;
    ~StandardAllocator() = delete;
};


template<typename SizeTy, SizeTy MaxSize>
struct FixedAllocator
{
    template <typename T>
    class Allocator
    {
    public:
        using value_type = T;
        using size_type = SizeTy;
        using difference_type = size_type;

        Allocator() = default;
        Allocator(const Allocator& ) noexcept {}
        ~Allocator() = default;

        NODISCARD value_type* allocate(const size_type size)
        {
            ASSERT(size <= MaxSize);
            return static_cast<value_type*>(buffer_->GetData());
        }

        void deallocate(value_type* const ptr, const size_type size)
        {
        }

        size_type max_size() const
        {
            return MaxSize;
        }

    private:
        UntypedData<T> buffer_[MaxSize];

    };

    FixedAllocator() = delete;
    ~FixedAllocator() = delete;
};

template<typename SizeTy, SizeTy InlineSize>
struct InlineAllocator
{
    template<typename T>
    class Allocator
    {
    public:
        using value_type = T;
        using size_type = SizeTy;
        using difference_type = size_type;

        Allocator() = default;
        Allocator(const Allocator& ) noexcept {}
        ~Allocator() = default;

        NODISCARD value_type* allocate(const size_type size)
        {
            if (size <= InlineSize)
            {
                return static_cast<value_type*>(buffer_->GetData());
            }
            else
            {
                return static_cast<value_type*>(allocate_impl<alignof(T)>(details::GetByteSize<sizeof(T)>(size)));
            }
        }

        void deallocate(value_type* const ptr, const size_type size)
        {
            if (size > InlineSize)
            {
                deallocate_impl<alignof(T)>(ptr);
            }
        }

    private:
        template <size_t Align, std::enable_if_t<(Align <= __STDCPP_DEFAULT_NEW_ALIGNMENT__), int> = 0>
        void* allocate_impl(size_t byte_size)
        {
            return Memory::Malloc(byte_size);
        }

        template <size_t Align, std::enable_if_t<(Align > __STDCPP_DEFAULT_NEW_ALIGNMENT__), int> = 0>
        void* allocate_impl(size_t byte_size)
        {
            return Memory::AlignedMalloc(byte_size, Align);
        }

        template <size_t Align, std::enable_if_t<(Align <= __STDCPP_DEFAULT_NEW_ALIGNMENT__), int> = 0>
        void deallocate_impl(void* ptr)
        {
            Memory::Free(ptr);
        }

        template <size_t Align, std::enable_if_t<(Align > __STDCPP_DEFAULT_NEW_ALIGNMENT__), int> = 0>
        void deallocate_impl(void* ptr)
        {
            Memory::AlignedFree(ptr);
        }

        UntypedData<T> buffer_[InlineSize];
    };
};

}
