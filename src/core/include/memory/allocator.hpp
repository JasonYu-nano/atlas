// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <memory>

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
size_t constexpr get_byte_size(const size_t count)
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

template <typename AllocatorType, typename = void>
struct HasInitializeSize : std::false_type {};

template <typename AllocatorType>
struct HasInitializeSize<AllocatorType,
                        std::void_t<decltype(std::declval<AllocatorType>().get_initialize_size())>> : std::true_type {};
}

template<typename AllocatorType>
struct AllocatorTraits : public std::allocator_traits<AllocatorType>
{
private:
    using base = std::allocator_traits<AllocatorType>;
public:
    static constexpr base::size_type get_initialize_size(const AllocatorType& allocator)
    {
        if constexpr (details::HasInitializeSize<AllocatorType>::value)
        {
            return allocator.get_initialize_size();
        }
        return 0;
    }

    static constexpr base::pointer reallocate(AllocatorType& allocator, base::pointer ptr, base::size_type old_count, base::size_type new_count)
    {
        if constexpr (details::HasReallocate<AllocatorType, typename base::pointer, typename base::size_type>::value)
        {
            return allocator.reallocate(ptr, old_count, new_count);
        }
        else
        {
            typename base::pointer new_ptr = base::allocate(allocator, new_count);
            std::memmove(new_ptr, ptr, details::get_byte_size<sizeof(base::value_type)>(math::min(old_count, new_count)));
            base::deallocate(allocator, ptr, old_count);
            return new_ptr;
        }
    }
};

/**
 * @brief Helper template for std::rebind_alloc
 * @tparam Allocator
 * @tparam NewType
 */
template<typename Allocator, typename NewType>
struct AllocatorRebind
{
    using type = AllocatorTraits<Allocator>::template rebind_alloc<NewType>;
};

template<typename T, typename SizeType = size_t>
class HeapAllocator
{
public:
    using value_type = T;
    using size_type = SizeType;
    using difference_type = std::make_signed_t<size_type>;

    constexpr HeapAllocator() noexcept = default;

    constexpr HeapAllocator(const HeapAllocator&) noexcept = default;

    template<typename Other>
    constexpr explicit HeapAllocator(const HeapAllocator<SizeType, Other>&) noexcept {}

    constexpr ~HeapAllocator() noexcept = default;

    bool operator==(const HeapAllocator&) const
    {
        return true;
    }

    NODISCARD constexpr value_type* allocate(const size_type size)
    {
        return static_cast<value_type*>(allocate_impl<alignof(T)>(details::get_byte_size<sizeof(T)>(size)));
    }

    NODISCARD constexpr value_type* reallocate(value_type* ptr, const size_type old_size, const size_type size)
    {
        return static_cast<value_type*>(reallocate_impl<alignof(T)>(ptr, details::get_byte_size<sizeof(T)>(size)));
    }

    constexpr void deallocate(value_type* const ptr, const size_type size)
    {
        deallocate_impl<alignof(T)>(ptr);
    }

private:
    template<size_t Align, std::enable_if_t<(Align <= __STDCPP_DEFAULT_NEW_ALIGNMENT__), int> = 0>
    constexpr void* allocate_impl(size_t byte_size)
    {
        return Memory::malloc(byte_size);
    }

    template<size_t Align, std::enable_if_t<(Align > __STDCPP_DEFAULT_NEW_ALIGNMENT__), int> = 0>
    constexpr void* allocate_impl(size_t byte_size)
    {
        return Memory::aligned_malloc(byte_size, Align);
    }

    template<size_t Align, std::enable_if_t<(Align <= __STDCPP_DEFAULT_NEW_ALIGNMENT__), int> = 0>
    constexpr void* reallocate_impl(value_type* ptr, size_t byte_size)
    {
        return Memory::realloc(ptr, byte_size);
    }

    template<size_t Align, std::enable_if_t<(Align > __STDCPP_DEFAULT_NEW_ALIGNMENT__), int> = 0>
    constexpr void* reallocate_impl(value_type* ptr, size_t byte_size)
    {
        return Memory::aligned_realloc(ptr, byte_size, Align);
    }

    template<size_t Align, std::enable_if_t<(Align <= __STDCPP_DEFAULT_NEW_ALIGNMENT__), int> = 0>
    constexpr void deallocate_impl(void* ptr)
    {
        Memory::free(ptr);
    }

    template<size_t Align, std::enable_if_t<(Align > __STDCPP_DEFAULT_NEW_ALIGNMENT__), int> = 0>
    constexpr void deallocate_impl(void* ptr)
    {
        Memory::aligned_free(ptr);
    }
};

template<typename T, uint32 MaxSize, typename SizeType = size_t>
class StackAllocator
{
public:
    using value_type = T;
    using size_type = SizeType;
    using difference_type = std::make_signed_t<size_type>;

    template<typename Other>
    struct rebind
    {
        using other = StackAllocator<Other, MaxSize, size_type>;
    };

    constexpr StackAllocator() noexcept = default;
    constexpr StackAllocator(const StackAllocator& ) noexcept {}
    constexpr ~StackAllocator() noexcept = default;

    bool operator==(const StackAllocator&) const
    {
        return false;
    }

    NODISCARD constexpr value_type* allocate(const size_type size)
    {
        ASSERT(size <= MaxSize);
        return static_cast<value_type*>(buffer_->data());
    }

    NODISCARD constexpr value_type* reallocate(value_type* ptr, const size_type old_size, const size_type size)
    {
        ASSERT(size <= MaxSize);
        return static_cast<value_type*>(buffer_->data());
    }

    constexpr void deallocate(value_type* const ptr, const size_type size)
    {
    }

    constexpr size_type max_size() const
    {
        return MaxSize;
    }

    constexpr size_type get_initialize_size() const
    {
        return MaxSize;
    }

private:
    UntypedData<T> buffer_[MaxSize];
};

template<typename T, uint32 InlineSize, typename SizeType = size_t, typename SecondaryAllocator = HeapAllocator<T>>
class InlineAllocator
{
    struct InlineBuffer
    {
        UntypedData<T> buffer_[InlineSize];
    };

public:
    using value_type = T;
    using size_type = SizeType;
    using difference_type = std::make_signed_t<size_type>;
    using secondary_allocator = AllocatorRebind<SecondaryAllocator, T>::type;

    template<typename Other>
    struct rebind
    {
        using other = InlineAllocator<Other, InlineSize, size_type, secondary_allocator>;
    };

    constexpr InlineAllocator() noexcept = default;
    constexpr InlineAllocator(const InlineAllocator& ) noexcept {}
    constexpr ~InlineAllocator() noexcept = default;

    bool operator==(const InlineAllocator&) const
    {
        return false;
    }

    NODISCARD constexpr value_type* allocate(const size_type size)
    {
        if (size <= InlineSize)
        {
            return static_cast<value_type*>(pair_.second().buffer_->data());
        }
        else
        {
            return pair_.first().allocate(size);
        }
    }

    NODISCARD constexpr value_type* reallocate(value_type* ptr, const size_type old_size, const size_type size)
    {
        if (old_size <= InlineSize && size <= InlineSize)
        {
            return static_cast<value_type*>(pair_.second().buffer_->data());
        }
        if (old_size <= InlineSize && size > InlineSize)
        {
            value_type* new_ptr = pair_.first().allocate(size);
            std::memmove(new_ptr, ptr, details::get_byte_size<sizeof(value_type)>(old_size));
            return new_ptr;
        }
        if (old_size > InlineSize && size <= InlineSize)
        {
            value_type* new_ptr = static_cast<value_type*>(pair_.second().buffer_->data());
            std::memmove(new_ptr, ptr, details::get_byte_size<sizeof(value_type)>(size));
            pair_.first().deallocate(ptr, old_size);
            return new_ptr;
        }

        return pair_.first().reallocate(ptr, old_size, size);
    }

    constexpr void deallocate(value_type* const ptr, const size_type size)
    {
        if (size > InlineSize)
        {
            return pair_.first().deallocate(ptr, size);
        }
    }

    constexpr size_type get_initialize_size() const
    {
        return InlineSize;
    }

private:
    CompressionPair<secondary_allocator, InlineBuffer> pair_;
};

}
