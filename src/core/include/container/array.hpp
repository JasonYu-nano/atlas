// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <span>
#include <type_traits>

#include "memory/allocator.hpp"
#include "core_macro.hpp"
#include "misc/iterator.hpp"
#include "utility/compression_pair.hpp"
#include "utility/untyped_data.hpp"

namespace atlas
{

namespace details
{
template<typename ValueType, typename SizeType>
struct ArrayVal
{
    SizeType size{ 0 };
    SizeType capacity{ 0 };
    ValueType* ptr{ nullptr };
};

template <typename RangeType>
concept ArrayRangeType = std::ranges::contiguous_range<RangeType> && requires (RangeType rng)
{
    rng.data();
};
}

template<typename T, typename Allocator = StandardAllocator<size_t>>
class Array
{
public:
    using value_type = T;
    using allocator_type = Allocator::template Allocator<value_type>;
    using allocator_traits = AllocatorTraits<allocator_type>;
    using size_type = allocator_traits::size_type;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using param_type = CallTraits<value_type>::param_type;
    using const_param_type = const param_type;
    using iterator = PointerIterator<pointer>;
    using const_iterator = PointerIterator<const_pointer>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    using val_type = details::ArrayVal<value_type, size_type>;

public:
    explicit Array(const allocator_type& alloc = allocator_type()) : pair_(alloc)
    {
        if (allocator_traits::get_initialize_size(pair_.First()) > 0)
        {
            Reserve(allocator_traits::get_initialize_size(pair_.First()));
        }
    }
    explicit Array(size_type capacity, const allocator_type& alloc = allocator_type()) : pair_(alloc)
    {
        Reserve(math::Max(capacity, allocator_traits::get_initialize_size(pair_.First())));
    }
    explicit Array(std::span<value_type> span, const allocator_type& alloc = allocator_type()) : pair_(alloc) { Construct(span); }
    Array(const std::initializer_list<value_type>& initializer, const allocator_type& alloc = allocator_type()) : pair_(alloc) { Construct(initializer); }
    Array(const_param_type value, size_type count, const allocator_type& alloc = allocator_type()) : pair_(alloc) { Construct(value, count); }
    Array(const Array& right) : pair_(allocator_traits::select_on_container_copy_construction(right.GetAlloc())) { Construct(right); }
    Array(Array&& right) noexcept
        : pair_(std::move(right.GetAlloc())
        , { std::exchange(right.GetVal().size, 0)
        , std::exchange(right.GetVal().capacity, 0)
        , std::exchange(right.GetVal().ptr, nullptr) })
    {}
    ~Array();

    NODISCARD pointer operator[] (size_type index) { return Data()[index]; }
    NODISCARD const_pointer operator[] (size_type index) const { return Data()[index]; }

    NODISCARD size_type Size() const { return GetVal().size; }
    NODISCARD DO_NOT_USE_DIRECTLY size_type size() const { return Size(); }

    NODISCARD constexpr size_type MaxSize() const { return allocator_traits::max_size(GetAlloc()); }
    NODISCARD DO_NOT_USE_DIRECTLY constexpr size_type max_size() const { return MaxSize(); }

    NODISCARD pointer Data() { return GetVal().ptr; }
    NODISCARD const_pointer Data() const { return GetVal().ptr; }
    NODISCARD DO_NOT_USE_DIRECTLY pointer data() { return GetVal().ptr; }
    NODISCARD DO_NOT_USE_DIRECTLY const_pointer data() const { return GetVal().ptr; }

    NODISCARD size_type Capacity() const { return GetVal().capacity; }
    NODISCARD bool Empty() const { return Size() <= 0; }

    size_type Add(const param_type elem) { return Emplace(elem); }
    size_type Add(value_type&& elem) { return Emplace(elem); }
    size_type AddUnique(const param_type elem) { if (Find(elem) != INDEX_NONE) Emplace(elem); }
    size_type AddUnique(value_type&& elem) { if (Find(elem) != INDEX_NONE) Emplace(std::forward<value_type>(elem)); }

    template<typename... Args>
    size_type Emplace(Args&&... args);

    size_type Append(std::span<value_type> elems);
    template<details::ArrayRangeType RangeType>
    size_type Append(const RangeType& others);
    template<typename AllocatorType>
    size_type Append(Array<value_type, AllocatorType>&& others);

    void Insert(const const_iterator& where, const param_type elem);
    void Insert(const const_iterator& where, value_type&& elem);
    void Insert(const const_iterator& where, std::span<value_type> elems);
    template<typename RangeType>
    void Insert(const const_iterator& where, const RangeType& elems)
    {
        if constexpr (std::ranges::sized_range<RangeType> || std::ranges::forward_range<RangeType>)
        {
            InsertCountedRange(where, elems.data(), std::ranges::distance(elems));
        }
    }
    template<typename AllocatorType>
    void Insert(const const_iterator& where, Array<value_type, AllocatorType>&& elems);

    void Remove(const param_type elem);
    void Remove(const std::function<bool(const param_type)>& predicate);
    void RemoveSwap(const param_type elem);
    void RemoveSwap(const std::function<bool(const param_type)>& predicate);
    void RemoveAll(const param_type elem);
    void RemoveAll(const std::function<bool(const param_type)>& predicate);
    void RemoveAllSwap(const param_type elem);
    void RemoveAllSwap(const std::function<bool(const param_type)>& predicate);
    void RemoveAt(const const_pointer& where);
    void RemoveAt(const const_pointer& where, size_type count);
    void RemoveAtSwap(const const_pointer& where);
    void RemoveAtSwap(const const_pointer& where, size_type count);

    NODISCARD size_type Find(const param_type search) const;
    NODISCARD size_type Find(const std::function<bool(const param_type)>& predicate) const;
    NODISCARD size_type FindLast(const param_type search) const;
    NODISCARD size_type FindLast(const std::function<bool(const param_type)>& predicate) const;

    void Reserve(size_type capacity);
    void Clear(bool reset_capacity = false);
    void ShinkToFit();

    NODISCARD iterator begin() { return iterator(Data()); }
    NODISCARD const_iterator begin() const{ return const_iterator(Data()); }
    NODISCARD iterator end() { return iterator(Data() + Size()); }
    NODISCARD const_iterator end() const { return const_iterator(Data() + Size()); }

    NODISCARD reverse_iterator rbegin() { return reverse_iterator(end()); }
    NODISCARD const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    NODISCARD reverse_iterator rend() { return reverse_iterator(begin()); }
    NODISCARD const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

    NODISCARD const_iterator cbegin() const { return begin(); }
    NODISCARD const_iterator cend() const { return end(); }
    NODISCARD const_reverse_iterator crbegin() const { return const_reverse_iterator(end()); }
    NODISCARD const_reverse_iterator crend() const { return const_reverse_iterator(begin()); }
private:
    allocator_type& GetAlloc() { return pair_.First(); }
    const allocator_type& GetAlloc() const { return pair_.First(); }

    val_type& GetVal() { return pair_.Second(); }
    const val_type& GetVal() const { return pair_.Second(); }

    template<typename Iter>
    void MoveToUninitialized(Iter first, Iter last, pointer dest);
    template<typename Iter>
    void CopyToUninitialized(Iter first, Iter last, pointer dest);
    template<typename Iter>
    void InsertCountedRange(const_iterator first, Iter it, size_type count);

    template<std::ranges::sized_range RangeType>
    void Construct(const RangeType& range);
    void Construct(const_param_type value, size_type count);
    template<std::ranges::sized_range RangeType>
    void MoveConstruct(const RangeType& range);

    size_type CalculateGrowth(size_type requested) const;
    size_type Reallocate(size_type new_capacity);
    size_type AddUninitialized(size_type& increase_size);

    template<std::integral SizeType>
    constexpr size_type ConvertSize(SizeType size)
    {
        ASSERT(size <= MaxSize());
        if constexpr (!std::is_same_v<SizeType, size_type>)
        {
            return static_cast<size_type>(size);
        }
        return size;
    }

    CompressionPair<allocator_type, val_type> pair_;
};

template<typename T, size_t N>
using InlineArray = Array<T, InlineAllocator<size_t, N>>;

template<typename T, size_t N>
using FixedArray = Array<T, FixedAllocator<size_t, N>>;

template<typename T, typename Allocator>
Array<T, Allocator>::~Array()
{
    auto&& my_val = GetVal();

    while (my_val.size > 0)
    {
        --my_val.size;
        std::destroy_at(my_val.ptr + my_val.size);
    }

    if (my_val.capacity > 0)
    {
        allocator_traits::deallocate(GetAlloc(), my_val.ptr, my_val.capacity);
    }
    my_val.ptr = nullptr;
    my_val.size = 0;
    my_val.capacity = 0;
}

template<typename T, typename Allocator>
template<typename... Args>
Array<T, Allocator>::size_type Array<T, Allocator>::Emplace(Args&& ... args)
{
    size_type increase = 1;
    size_type index = AddUninitialized(increase);
    if (index != INDEX_NONE)
    {
        new (Data() + index) value_type(std::forward<Args>(args)...);
    }
    return index;
}

template<typename T, typename Allocator>
Array<T, Allocator>::size_type Array<T, Allocator>::Append(std::span<value_type> elems)
{
    size_type increase_size = ConvertSize(elems.size());
    size_type index = AddUninitialized(increase_size);
    if constexpr (std::is_trivially_copyable<value_type>::value)
    {
        std::memmove(Data() + index, elems.data(), increase_size * sizeof(value_type));
    }
    else
    {
        pointer ptr = Data() + index;
        pointer source = elems.data();
        for (size_type i = 0; i < increase_size; ++i)
        {
            new (ptr + i) value_type(source[i]);
        }
    }
    return index;
}

template<typename T, typename Allocator>
template<details::ArrayRangeType RangeType>
Array<T, Allocator>::size_type Array<T, Allocator>::Append(const RangeType& others)
{
    size_type increase_size = others.size();
    size_type index = AddUninitialized(increase_size);
    if constexpr (std::is_trivially_copyable<value_type>::value)
    {
        std::memmove(Data() + index, others.data(), increase_size * sizeof(value_type));
    }
    else
    {
        pointer ptr = Data() + index;
        const_pointer source = others.data();
        for (size_type i = 0; i < increase_size; ++i)
        {
            new (ptr + i) value_type(source[i]);
        }
    }

    return index;
}

template<typename T, typename Allocator>
template<typename AllocatorType>
Array<T, Allocator>::size_type Array<T, Allocator>::Append(Array<value_type, AllocatorType>&& others)
{
    size_type increase_size = others.Size();
    size_type index = AddUninitialized(increase_size);
    if constexpr (std::is_trivially_copyable<value_type>::value)
    {
        std::memmove(Data() + index, others.Data(), increase_size * sizeof(value_type));
    }
    else
    {
        pointer ptr = Data() + index;
        const_pointer source = others.Data();
        for (size_type i = 0; i < increase_size; ++i)
        {
            new (ptr + i) value_type(std::move(source[i]));
        }
    }
    auto&& other_val = others.GetVal();
    other_val.size = 0;

    return index;
}

template<typename T, typename Allocator>
Array<T, Allocator>::size_type Array<T, Allocator>::Find(const param_type search) const
{
    for (auto it = cbegin(); it < cend(); ++it)
    {
        if (*it == search)
        {
            return it - cbegin();
        }
    }
    return INDEX_NONE;
}

template<typename T, typename Allocator>
Array<T, Allocator>::size_type Array<T, Allocator>::Find(const std::function<bool(const param_type)>& predicate) const
{
    for (auto it = cbegin(); it < cend(); ++it)
    {
        if (predicate(*it))
        {
            return it - cbegin();
        }
    }
    return INDEX_NONE;
}

template<typename T, typename Allocator>
Array<T, Allocator>::size_type Array<T, Allocator>::FindLast(const param_type search) const
{
    for (auto it = crbegin(); it < crend(); ++it)
    {
        if (*it == search)
        {
            return crend() - it - 1;
        }
    }
    return INDEX_NONE;
}

template<typename T, typename Allocator>
Array<T, Allocator>::size_type Array<T, Allocator>::FindLast(const std::function<bool(const param_type)>& predicate) const
{
    for (auto it = crbegin(); it < crend(); ++it)
    {
        if (predicate(*it))
        {
            return crend() - it - 1;
        }
    }
    return INDEX_NONE;
}

template<typename T, typename AllocType>
void Array<T, AllocType>::Reserve(Array::size_type capacity)
{
    auto&& my_val = GetVal();
    if (capacity > my_val.capacity)
    {
        size_type new_capacity = CalculateGrowth(capacity);
        if (my_val.capacity <= 0)
        {
            my_val.ptr = allocator_traits::allocate(GetAlloc(), new_capacity);
        }
        else if constexpr (std::is_trivially_copyable<value_type>::value)
        {
            my_val.ptr = allocator_traits::reallocate(GetAlloc(), my_val.ptr, my_val.capacity, new_capacity);
        }
        else
        {
            allocator_type& allocator = GetAlloc();
            pointer new_ptr = allocator_traits::allocate(allocator, new_capacity);
            pointer old_ptr = my_val.ptr;
            if (new_ptr != old_ptr)
            {
                MoveToUninitialized(old_ptr, old_ptr + my_val.size, new_ptr);
                allocator_traits::deallocate(GetAlloc(), old_ptr, my_val.capacity);
            }
            my_val.ptr = new_ptr;
        }

        my_val.capacity = new_capacity;
    }
}

template<typename T, typename Allocator>
void Array<T, Allocator>::Clear(bool reset_capacity)
{
    auto&& my_val = GetVal();
    if (my_val.size > 0)
    {
        for (size_type i = 0; i < my_val.size; ++i)
        {
            std::destroy_at(my_val.ptr + i);
        }
        my_val.size = 0;
    }

    if (reset_capacity)
    {
        auto&& alloc = GetAlloc();
        size_type initialize_capacity = allocator_traits::get_initialize_size(alloc);
        if (my_val.capacity > initialize_capacity)
        {
            if (initialize_capacity <= 0)
            {
                allocator_traits::deallocate(alloc, my_val.ptr, my_val.capacity);
            }
            else
            {
                allocator_traits::reallocate(alloc, my_val.ptr, my_val.capacity, initialize_capacity);
            }
            my_val.capacity = initialize_capacity;
        }
    }
}

template<typename T, typename Allocator>
template<typename Iter>
void Array<T, Allocator>::MoveToUninitialized(Iter first, Iter last, pointer dest)
{
    if constexpr (std::is_trivially_copyable<value_type>::value)
    {
        std::memmove(static_cast<void*>(dest), static_cast<const void*>(first), (last - first) * sizeof(value_type));
    }
    else
    {
        auto&& alloc = GetAlloc();
        for (size_type idx = 0; first != last; ++first, ++idx)
        {
            allocator_traits::construct(alloc, dest + idx, std::move(*first));
        }
    }
}

template<typename T, typename Allocator>
template<typename Iter>
void Array<T, Allocator>::CopyToUninitialized(Iter first, Iter last, pointer dest)
{
    if constexpr (std::is_trivially_copyable<value_type>::value)
    {
        std::memmove(static_cast<void*>(dest), static_cast<const void*>(first), (last - first) * sizeof(value_type));
    }
    else
    {
        auto&& alloc = GetAlloc();
        for (size_type idx = 0; first != last; ++first, ++idx)
        {
            allocator_traits::construct(alloc, dest + idx, *first);
        }
    }
}

template<typename T, typename Allocator>
template<typename Iter>
void Array<T, Allocator>::InsertCountedRange(const_iterator where, Iter it, size_type count)
{
    if (count <= 0)
    {
        return;
    }

    auto&& my_val = GetVal();
    size_type unused_capacity = my_val.capacity - my_val.size;

    if (count <= unused_capacity)
    {
        const_iterator end = cend();
        size_type move_count = end - where;
        if (move_count > 0)
        {
            if (move_count <= count)
            {
                // move to uninitialized location
                MoveToUninitialized(where, end, (where + count).GetPointer());
                std::destroy(where, end);
            }
            else
            {
                MoveToUninitialized(end - count, end, end.GetPointer());
                std::move_backward(where, end - count, end);
                std::destroy(where, where + count);
            }
        }
        CopyToUninitialized(it, it + count, where.GetPointer());
        my_val.size += count;
    }
    else
    {
        if (count > MaxSize() - my_val.size)
        {
            ASSERT(0);
            count = MaxSize() - my_val.size;
        }

        size_type new_size = my_val.size + count;
        size_type new_capacity = CalculateGrowth(new_size);
        auto&& alloc = GetAlloc();

        pointer new_ptr = allocator_traits::allocate(alloc, new_capacity);
        const_iterator begin = begin();
        const_iterator end = end();
        size_type offset = where - begin;
        CopyToUninitialized(it, it + count, new_ptr + offset);
        if (where == end)
        {
            MoveToUninitialized(begin, end, new_ptr);
        }
        else
        {
            MoveToUninitialized(begin, where, new_ptr);
            MoveToUninitialized(where, end, new_ptr + offset + count);
        }
        std::destroy(begin, end);
        allocator_traits::deallocate(alloc, my_val.ptr, my_val.capacity);
        my_val.ptr = new_ptr;
        my_val.size = new_size;
        my_val.capacity = new_capacity;
    }
}

template<typename T, typename Allocator>
template<std::ranges::sized_range RangeType>
void Array<T, Allocator>::Construct(const RangeType& range)
{
    size_type size = range.size();
    Reserve(size);
    pointer data = Data();

    if constexpr (std::is_trivially_copyable<value_type>::value)
    {
        Memory::Memmove(static_cast<void*>(data), static_cast<const void*>(&*range.begin()), size * sizeof(value_type));
    }
    else
    {
        for (auto&& it = range.begin(); it < range.end(); ++it, ++data)
        {
            new (data) T(*it);
        }
    }
    GetVal().size = size;
}

template<typename T, typename Allocator>
void Array<T, Allocator>::Construct(const_param_type value, size_type count)
{
    if (count > 0)
    {
        Reserve(count);
        pointer data = Data();

        for (int32 i = 0; i < count; ++i)
        {
            if constexpr (std::is_trivially_copyable<value_type>::value)
            {
                Memory::Memmove(static_cast<void*>(data + 1), static_cast<const void*>(&value), sizeof(value_type));
            }
            else
            {
                new (data + i) T(value);
            }
        }

        GetVal().size = count;
    }
}

template<typename T, typename Allocator>
template<std::ranges::sized_range RangeType>
void Array<T, Allocator>::MoveConstruct(const RangeType& range)
{
    size_type size = range.size();
    Reserve(size);
    MoveToUninitialized(range.begin(), range.end(), Data());
    GetVal().size = size;
}

template<typename T, typename Allocator>
Array<T, Allocator>::size_type Array<T, Allocator>::CalculateGrowth(Array::size_type requested) const
{
    if (requested > MaxSize())
    {
        return MaxSize();
    }

    size_type old = GetVal().capacity;
    if (old > MaxSize() - old)
    {
        return MaxSize();
    }

    return math::Max(requested, 2 * old);
}

template<typename T, typename Allocator>
Array<T, Allocator>::size_type Array<T, Allocator>::AddUninitialized(Array::size_type& increase_size)
{
    auto&& my_val = GetVal();
    size_type old_size = my_val.size;
    if (MaxSize() - old_size < increase_size)
    {
        ASSERT(0);
        increase_size = MaxSize() - old_size;
    }

    if (increase_size <= 0)
    {
        return static_cast<size_type>(INDEX_NONE);
    }

    size_type new_size = old_size + increase_size;
    if (new_size > my_val.capacity)
    {
        size_type new_capacity = CalculateGrowth(new_size);
        if (new_capacity > my_val.capacity)
        {
            auto&& alloc = GetAlloc();
            if (my_val.capacity <= 0)
            {
                my_val.ptr = allocator_traits::allocate(alloc, new_capacity);
            }
            else if constexpr (std::is_trivially_copyable<value_type>::value)
            {
                my_val.ptr = allocator_traits::reallocate(alloc, my_val.ptr, my_val.capacity, new_capacity);
            }
            else
            {
                pointer new_ptr = allocator_traits::allocate(alloc, new_capacity);
                pointer old_ptr = my_val.ptr;
                if (new_ptr != old_ptr)
                {
                    MoveToUninitialized(old_ptr, old_ptr + my_val.size, new_ptr);
                    allocator_traits::deallocate(alloc, old_ptr, my_val.capacity);
                }
                my_val.ptr = new_ptr;
            }
            my_val.capacity = new_capacity;
        }
    }

    my_val.size = new_size;
    return old_size;
}

}
