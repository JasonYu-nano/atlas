// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "assertion.hpp"
#include "memory/memory.hpp"
#include "utility/untyped_data.hpp"
#include "utility/compression_pair.hpp"
#include "math/atlas_math.hpp"

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

template <typename AllocatorType, typename PointerType, typename SizeType, typename = void>
struct HasReallocate : std::false_type {};

template <typename AllocatorType, typename PointerType, typename SizeType>
struct HasReallocate<AllocatorType, PointerType, SizeType,
                     std::void_t<decltype(std::declval<AllocatorType>().reallocate(std::declval<PointerType>(),
                                                                                   std::declval<SizeType>(),
                                                                                   std::declval<SizeType>()))>> : std::true_type {};

}

template<typename AllocatorType>
struct AllocatorTraits : public std::allocator_traits<AllocatorType>
{
private:
    using Super = std::allocator_traits<AllocatorType>;
public:
    static constexpr Super::pointer reallocate(AllocatorType& allocator, Super::pointer ptr, Super::size_type old_count, Super::size_type new_count)
    {
        if constexpr (details::HasReallocate<AllocatorType, typename Super::pointer, typename Super::size_type>::value)
        {
            return allocator.reallocate(ptr, old_count, new_count);
        }
        else
        {
            typename Super::pointer new_ptr = Super::allocate(allocator, new_count);
            std::memmove(new_ptr, ptr, details::GetByteSize<sizeof(Super::value_type)>(math::Min(old_count, new_count)));
            Super::deallocate(allocator, ptr, old_count);
            return new_ptr;
        }
    }
};

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

        constexpr Allocator() noexcept = default;
        constexpr Allocator(const Allocator&) noexcept = default;
        template<typename Other>
        constexpr Allocator(const Allocator<Other>&) noexcept {}
        constexpr ~Allocator() noexcept = default;

        NODISCARD constexpr value_type* allocate(const size_type size)
        {
            return static_cast<value_type*>(allocate_impl<alignof(T)>(details::GetByteSize<sizeof(T)>(size)));
        }

        NODISCARD constexpr value_type* reallocate(value_type* ptr, const size_type old_size, const size_type size)
        {
            return static_cast<value_type*>(reallocate_impl<alignof(T)>(ptr, details::GetByteSize<sizeof(T)>(size)));
        }

        constexpr void deallocate(value_type* const ptr, const size_type size)
        {
            deallocate_impl<alignof(T)>(ptr);
        }

    private:
        template <size_t Align, std::enable_if_t<(Align <= __STDCPP_DEFAULT_NEW_ALIGNMENT__), int> = 0>
        constexpr void* allocate_impl(size_t byte_size)
        {
            return Memory::Malloc(byte_size);
        }

        template <size_t Align, std::enable_if_t<(Align > __STDCPP_DEFAULT_NEW_ALIGNMENT__), int> = 0>
        constexpr void* allocate_impl(size_t byte_size)
        {
            return Memory::AlignedMalloc(byte_size, Align);
        }

        template <size_t Align, std::enable_if_t<(Align <= __STDCPP_DEFAULT_NEW_ALIGNMENT__), int> = 0>
        constexpr void* reallocate_impl(value_type* ptr, size_t byte_size)
        {
            return Memory::Realloc(ptr, byte_size);
        }

        template <size_t Align, std::enable_if_t<(Align > __STDCPP_DEFAULT_NEW_ALIGNMENT__), int> = 0>
        constexpr void* reallocate_impl(value_type* ptr, size_t byte_size)
        {
            return Memory::AlignedRealloc(ptr, byte_size, Align);
        }

        template <size_t Align, std::enable_if_t<(Align <= __STDCPP_DEFAULT_NEW_ALIGNMENT__), int> = 0>
        constexpr void deallocate_impl(void* ptr)
        {
            Memory::Free(ptr);
        }

        template <size_t Align, std::enable_if_t<(Align > __STDCPP_DEFAULT_NEW_ALIGNMENT__), int> = 0>
        constexpr void deallocate_impl(void* ptr)
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

        constexpr Allocator() noexcept = default;
        constexpr Allocator(const Allocator& ) noexcept {}
        constexpr ~Allocator() noexcept = default;

        NODISCARD constexpr value_type* allocate(const size_type size)
        {
            ASSERT(size <= MaxSize);
            return static_cast<value_type*>(buffer_->GetData());
        }

        NODISCARD constexpr value_type* reallocate(value_type* ptr, const size_type old_size, const size_type size)
        {
            ASSERT(size <= MaxSize);
            return static_cast<value_type*>(buffer_->GetData());
        }

        constexpr void deallocate(value_type* const ptr, const size_type size)
        {
        }

        constexpr size_type max_size() const
        {
            return MaxSize;
        }

    private:
        UntypedData<T> buffer_[MaxSize];

    };

    FixedAllocator() = delete;
    ~FixedAllocator() = delete;
};

template<typename SizeTy, SizeTy InlineSize, typename SecondaryAllocator = StandardAllocator<SizeTy>>
struct InlineAllocator
{
    template<typename T>
    class Allocator
    {
        struct InlineBuffer
        {
            UntypedData<T> buffer_[InlineSize];
        };

    public:
        using value_type = T;
        using size_type = SizeTy;
        using difference_type = size_type;
        using secondary_allocator = SecondaryAllocator::template Allocator<T>;

        constexpr Allocator() noexcept = default;
        constexpr Allocator(const Allocator& ) noexcept {}
        constexpr ~Allocator() noexcept = default;

        NODISCARD constexpr value_type* allocate(const size_type size)
        {
            if (size <= InlineSize)
            {
                return static_cast<value_type*>(pair_.Second().buffer_->GetData());
            }
            else
            {
                return pair_.First().allocate(size);
            }
        }

        NODISCARD constexpr value_type* reallocate(value_type* ptr, const size_type old_size, const size_type size)
        {
            if (old_size <= InlineSize && size <= InlineSize)
            {
                return static_cast<value_type*>(pair_.Second().buffer_->GetData());
            }
            if (old_size <= InlineSize && size > InlineSize)
            {
                value_type* new_ptr = pair_.First().allocate(size);
                std::memmove(new_ptr, ptr, details::GetByteSize<sizeof(value_type)>(old_size));
                return new_ptr;
            }
            if (old_size > InlineSize && size <= InlineSize)
            {
                value_type* new_ptr = static_cast<value_type*>(pair_.Second().buffer_->GetData());
                std::memmove(new_ptr, ptr, details::GetByteSize<sizeof(value_type)>(size));
                pair_.First().deallocate(ptr, old_size);
                return new_ptr;
            }

            return pair_.First().reallocate(ptr, old_size, size);
        }

        constexpr void deallocate(value_type* const ptr, const size_type size)
        {
            if (size > InlineSize)
            {
                return pair_.First().deallocate(ptr, size);
            }
        }

    private:
        CompressionPair<secondary_allocator, InlineBuffer> pair_;
    };
};

}
