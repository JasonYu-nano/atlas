// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

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
} // namespace details

template<typename T, typename Allocator = HeapAllocator<T>>
class Array
{
public:
    using value_type = T;
    using allocator_type = AllocatorRebind<Allocator, T>::type;
    using allocator_traits = AllocatorTraits<allocator_type>;
    using size_type = allocator_traits::size_type;
    using difference_type = allocator_traits::difference_type;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = CallTraits<value_type>::reference;
    using const_reference = CallTraits<value_type>::const_reference;
    using param_type = CallTraits<value_type>::param_type;
    using const_param_type = const param_type;
    using iterator = PointerIterator<value_type>;
    using const_iterator = ConstPointerIterator<value_type>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    using val_type = details::ArrayVal<value_type, size_type>;

public:
    /**
     * @brief Constructor.
     * @param alloc
     */
    explicit Array(const allocator_type& alloc = allocator_type()) : pair_(alloc)
    {
        Reserve(GetInitializeCapacity());
    }
    /**
     * @brief Constructor, initialize with given capacity.
     * @param capacity
     * @param alloc
     */
    explicit Array(size_type capacity, const allocator_type& alloc = allocator_type()) : pair_(alloc)
    {
        Reserve(math::Max(capacity, GetInitializeCapacity()));
    }
    /**
     * @brief Constructor from an initializer
     * @param initializer
     * @param alloc
     */
    Array(const std::initializer_list<value_type>& initializer, const allocator_type& alloc = allocator_type()) : pair_(alloc)
    {
        Construct(initializer);
    }
    /**
     * @brief Constructor from a range
     * @tparam RangeType
     * @param range
     * @param alloc
     */
    template<std::ranges::forward_range RangeType>
    explicit Array(const RangeType& range, const allocator_type& alloc = allocator_type()) : pair_(alloc)
    {
        Construct(range);
    }
    /**
     * @brief Constructor from given number elements.
     * @param value
     * @param count
     * @param alloc
     */
    Array(const_param_type value, size_type count, const allocator_type& alloc = allocator_type()) : pair_(alloc)
    {
        Construct(value, count);
    }
    /**
     * @brief Copy constructor.
     * @param right
     */
    Array(const Array& right) : pair_(allocator_traits::select_on_container_copy_construction(right.GetAlloc()))
    {
        Construct(right);
    }
    /**
     * @brief Copy constructs from other allocator type array.
     * @tparam OtherAllocator
     * @param right
     * @param alloc
     */
    template<typename OtherAllocator>
    Array(const Array<value_type, OtherAllocator>& right, const allocator_type& alloc = allocator_type()) : pair_(alloc)
    {
        Construct(right);
    }
    /**
     * @brief Move constructor.
     * @param right
     */
    Array(Array&& right) noexcept
        : pair_(OneThenVariadicArgs()
        , std::move(right.GetAlloc())
        , std::exchange(right.GetVal().size, 0)
        , std::exchange(right.GetVal().capacity, 0)
        , std::exchange(right.GetVal().ptr, nullptr))
    {}
    /**
     * @brief Move constructs from other allocator type array.
     * @tparam OtherAllocator
     * @param right
     * @param alloc
     */
    template<typename OtherAllocator>
    Array(Array<value_type, OtherAllocator>&& right, const allocator_type& alloc = allocator_type()) : pair_(alloc)
    {
        MoveAssign(right);
    }

    ~Array() noexcept
    {
        Clear(true);
    }

    Array& operator= (const Array& right)
    {
        if (this != std::addressof(right))
        {
            CopyAssignAllocator(right.GetAlloc());
            CopyAssign(right);
        }
        return *this;
    }

    Array& operator= (Array&& right) noexcept
    {
        if (this != std::addressof(right))
        {
            MoveAssign(right, std::integral_constant<bool,
                allocator_traits::propagate_on_container_move_assignment::value>());
        }
        return *this;
    }

    template<typename OtherAllocator>
    Array& operator= (const Array<value_type, OtherAllocator>& right)
    {
        CopyAssign(right);
        return *this;
    }

    template<typename OtherAllocator>
    Array& operator= (Array<value_type, OtherAllocator>&& right)
    {
        MoveAssign(right);
        return *this;
    }
    /**
     * @brief Get reference to element at given index.
     * @param index
     * @return
     */
    NODISCARD reference operator[] (size_type index)
    {
        ASSERT(IsValidIndex(index));
        return Data()[index];
    }
    /**
     * @brief Get const reference to element at given index.
     * @param index
     * @return
     */
    NODISCARD const_reference operator[] (size_type index) const
    {
        ASSERT(IsValidIndex(index));
        return Data()[index];
    }
    /**
     * @brief Get number of elements in array.
     * @return
     */
    NODISCARD size_type Size() const { return GetVal().size; }
    NODISCARD DO_NOT_USE_DIRECTLY size_type size() const { return Size(); }
    /**
     * @brief Get maximum number of elements in array.
     * @return
     */
    NODISCARD constexpr size_type MaxSize() const { return allocator_traits::max_size(GetAlloc()); }
    NODISCARD DO_NOT_USE_DIRECTLY constexpr size_type max_size() const { return MaxSize(); }
    /**
     * @brief Returns pointer to the first array element.
     * @return
     */
    NODISCARD pointer Data() { return GetVal().ptr; }
    /**
     * @brief Returns const pointer to the first array element.
     * @return
     */
    NODISCARD const_pointer Data() const { return GetVal().ptr; }
    NODISCARD DO_NOT_USE_DIRECTLY pointer data() { return GetVal().ptr; }
    NODISCARD DO_NOT_USE_DIRECTLY const_pointer data() const { return GetVal().ptr; }
    /**
     * @brief Get last index of array.
     * @return
     */
    NODISCARD size_type LastIndex() const
    {
        ASSERT(Size() > 0);
        return Size() - 1;
    }
    /**
     * @brief Get n-th last element from the array.
     * @return
     */
    NODISCARD reference Last(size_type index_from_end = 0)
    {
        return Data()[LastIndex() - index_from_end];
    }
    /**
     * @brief Get n-th last element from the array.
     * @return
     */
    NODISCARD const_reference Last(size_type index_from_end = 0) const
    {
        return Data()[LastIndex() - index_from_end];
    }
    /**
     * @brief Get capacity of array
     * @return
     */
    NODISCARD size_type Capacity() const { return GetVal().capacity; }
    /**
     * @brief Returns true if the array is empty and contains no elements.
     * @return
     */
    NODISCARD bool IsEmpty() const { return Size() <= 0; }
    /**
     * @brief Tests if index is valid.
     * @return
     */
    NODISCARD bool IsValidIndex(size_type index) const { return index >= 0 && index < Size(); }
    /**
     * @brief Copies a new element to the end of the array, possibly reallocating the whole array to fit.
     * @param elem
     * @return Index to the new element
     */
    size_type Add(const param_type elem) { return Emplace(elem); }
    /**
     * @brief Moves a new element to the end of the array, possibly reallocating the whole array to fit.
     * @param elem
     * @return Index to the new element
     */
    size_type Add(value_type&& elem) { return Emplace(elem); }
    /**
     * @brief Copies unique element to array if it doesn't exist.
     * @param elem
     * @return Index to the element
     */
    size_type AddUnique(const param_type elem)
    {
        size_type index = Find(elem);
        return index != INDEX_NONE ? index : Emplace(elem);
    }
    /**
     * @brief Moves unique element to array if it doesn't exist.
     * @param elem
     * @return Index to the element
     */
    size_type AddUnique(value_type&& elem)
    {
        size_type index = Find(elem);
        return index != INDEX_NONE ? index : Emplace(std::forward<value_type>(elem));
    }
    /**
     * @brief Constructs a new item at the end of the array, possibly reallocating the whole array to fit.
     * @tparam Args
     * @param args
     * @return Index to the new element
     */
    template<typename... Args>
    size_type Emplace(Args&&... args)
    {
        size_type increase = 1;
        size_type index = AddUninitialized(increase);
        if (index != INDEX_NONE)
        {
            new (Data() + index) value_type(std::forward<Args>(args)...);
        }
        return index;
    }
    /**
     * @brief Appends a range of elements to this array.
     * @tparam RangeType
     * @param others
     * @return Index to the first append element
     */
    template<std::ranges::forward_range RangeType>
    size_type Append(const RangeType& others)
    {
        size_type increase_size = ConvertSize(std::ranges::distance(others));
        size_type index = AddUninitialized(increase_size);
        if constexpr (std::is_trivially_copyable<value_type>::value)
        {
            std::memmove(Data() + index, IteratorToPointer(std::ranges::begin(others)), increase_size * sizeof(value_type));
        }
        else
        {
            pointer ptr = Data() + index;
            auto&& it = std::ranges::begin(others);
            for (size_type i = 0; i < increase_size; ++i, ++it)
            {
                new (ptr + i) value_type(*it);
            }
        }

        return index;
    }
    /**
     * @brief Appends a range of elements to this array.
     * @tparam AllocatorType
     * @param others
     * @return Index to the first append element
     */
    template<typename AllocatorType>
    size_type Append(Array<value_type, AllocatorType>&& others)
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
    /**
     * @brief Inserts given element into the array at given position.
     * @param where
     * @param elem
     */
    iterator Insert(const_iterator where, const param_type elem)
    {
        return InsertCountedRange(where, &elem, 1);
    }
    /**
     * @brief Inserts given element into the array at given position.
     * @param where
     * @param elem
     */
    iterator Insert(const_iterator where, value_type&& elem)
    {
        return InsertCountedRange(where, &elem, 1, true);
    }
    /**
     * @brief Inserts given elements into the array at given position.
     * @param where
     * @param elem
     */
    template<std::ranges::range RangeType>
    iterator Insert(const_iterator where, const RangeType& elems)
    {
        if constexpr (std::ranges::sized_range<RangeType> || std::ranges::forward_range<RangeType>)
        {
            return InsertCountedRange(where, std::ranges::begin(elems), std::ranges::distance(elems));
        }
        return begin() + (where - cbegin());
    }
    /**
     * @brief Inserts other array into the array at given position.
     * @param where
     * @param elem
     */
    template<typename AllocatorType>
    iterator Insert(const_iterator where, Array<value_type, AllocatorType>&& elems)
    {
        return InsertCountedRange(where, elems.begin(), elems.Size(), true);
    }
    /**
     * @brief Removes first matched element in the array.
     * @param elem
     * @return Index of removed element
     */
    size_type Remove(const param_type elem)
    {
        size_type index = Find(elem);
        if (index != INDEX_NONE)
        {
            RemoveAt(cbegin() + index);
        }
        return index;
    }
    /**
     * @brief Removes first matched element in the array.
     * @param predicate
     * @return Index of removed element
     */
    template<typename Predicate>
    size_type Remove(const Predicate& predicate) requires std::predicate<Predicate, const param_type>
    {
        size_type index = Find(predicate);
        if (index != INDEX_NONE)
        {
            RemoveAt(cbegin() + index);
        }
        return index;
    }
    /**
     * @brief Removes first matched element in the array. It's faster than Remove but breaks the order
     * @param elem
     * @return Index of removed element
     */
    size_type RemoveSwap(const param_type elem)
    {
        size_type index = Find(elem);
        if (index != INDEX_NONE)
        {
            RemoveAtSwap(cbegin() + index);
        }
        return index;
    }
    /**
     * @brief Removes first matched element in the array. It's faster than Remove but breaks the order
     * @param predicate
     * @return Index of removed element
     */
    template<typename Predicate>
    size_type RemoveSwap(const Predicate& predicate) requires std::predicate<Predicate, const param_type>
    {
        size_type index = Find(predicate);
        if (index != INDEX_NONE)
        {
            RemoveAtSwap(cbegin() + index);
        }
        return index;
    }
    /**
     * @brief Removes all instances in the array that matched element.
     * @param elem
     * @return Numbers of removed elements
     */
    size_type RemoveAll(const param_type elem)
    {
        return RemoveAll([&elem](const param_type value) { return value == elem; });
    }
    /**
     * @brief Removes all instances in the array that matched element.
     * @param predicate
     * @return Numbers of removed elements
     */
    template<typename Predicate>
    size_type RemoveAll(const Predicate& predicate) requires std::predicate<Predicate, const param_type>
    {
        auto&& my_val = GetVal();
        size_type num_of_matches = 0;
        if (my_val.size > 0)
        {
            pointer read = my_val.ptr;
            pointer write = read;
            pointer last = my_val.ptr + my_val.size;
            bool not_match = !predicate(*read);
            do
            {
                pointer start = read++;
                while (read < last && not_match == !predicate(*read))
                {
                    ++read;
                }

                if (not_match)
                {
                    if (write != start)
                    {
                        std::move(start, read, write);
                    }
                    write += (read - start);
                }
                else
                {
                    num_of_matches += read - start;
                }
                not_match = !not_match;
            }
            while (read < last);

            if (num_of_matches > 0)
            {
                std::destroy(last - num_of_matches, last);
                my_val.size -= num_of_matches;
            }
        }

        return num_of_matches;
    }
    /**
     * @brief Removes all instances in the array that matched element. It's faster than RemoveAll but breaks the order
     * @param elem
     * @return Numbers of removed elements
     */
    size_type RemoveAllSwap(const param_type elem)
    {
        return RemoveAllSwap([&elem](const param_type value) { return value == elem; });
    }
    /**
     * @brief Removes all instances in the array that matched element. It's faster than RemoveAll but breaks the order
     * @param predicate
     * @return Numbers of removed elements
     */
    template<typename Predicate>
    size_type RemoveAllSwap(const Predicate& predicate) requires std::predicate<Predicate, const param_type>
    {
        size_type num_of_matches = 0;
        if (Size() > 0)
        {
            const_iterator read = cbegin();
            const_iterator last = cend();

            while (read < last)
            {
                if (predicate(*read))
                {
                    RemoveAtSwap(read);
                    ++num_of_matches;
                    --last;
                }
                else
                {
                    ++read;
                }
            }
        }

        return num_of_matches;
    }
    /**
     * @brief Removes element at given position.
     * @param where
     * @return Next element iterator
     */
    iterator RemoveAt(size_type where)
    {
        return RemoveAt(begin() + where, 1);
    }
    /**
     * @brief Removes multiple elements at given position.
     * @param where
     * @param count
     * @return Next element iterator
     */
    iterator RemoveAt(size_type where, size_type count)
    {
        return RemoveAt(begin() + where, count);
    }
    /**
     * @brief Removes element at given position.
     * @param where
     * @return Next element iterator
     */
    iterator RemoveAt(const_iterator where)
    {
        return RemoveAt(where, 1);
    }
    /**
     * @brief Removes multiple elements at given position.
     * @param where
     * @param count
     * @return Next element iterator
     */
    iterator RemoveAt(const_iterator where, size_type count)
    {
        if (count <= 0)
        {
            return iterator(const_cast<value_type*>(IteratorToPointer(where)));
        }
        ASSERT(cbegin() <= where && where < cend() && where + count <= cend());
        auto&& my_val = GetVal();
        pointer data = my_val.ptr;
        size_type offset = where - cbegin();
        pointer location = data + offset;
        pointer end = data + my_val.size;
        std::move(location + count, end, location);
        std::destroy(end - count, end);
        my_val.size -= count;
        return iterator(location);
    }
    /**
     * @brief Removes element at given position. It's faster than RemoveAt but breaks the order
     * @param where
     * @return Next element iterator
     */
    iterator RemoveAtSwap(size_type where)
    {
        return RemoveAtSwap(begin() + where, 1);
    }
    /**
     * @brief Removes multiple elements at given position. It's faster than RemoveAt but breaks the order
     * @param where
     * @param count
     * @return Next element iterator
     */
    iterator RemoveAtSwap(size_type where, size_type count)
    {
        return RemoveAtSwap(begin() + where, count);
    }
    /**
     * @brief Removes element at given position. It's faster than RemoveAt but breaks the order
     * @param where
     * @return Next element iterator
     */
    iterator RemoveAtSwap(const_iterator where)
    {
        return RemoveAtSwap(where, 1);
    }
    /**
     * @brief Removes multiple elements at given position. It's faster than RemoveAt but breaks the order
     * @param where
     * @param count
     * @return Next element iterator
     */
    iterator RemoveAtSwap(const_iterator where, size_type count)
    {
        ASSERT(cbegin() <= where && where < cend() && where + count <= cend());
        auto&& my_val = GetVal();
        pointer data = my_val.ptr;
        size_type offset = where - cbegin();
        pointer location = data + offset;
        pointer end = data + my_val.size;
        size_type remain = end - location - count;
        if (remain <= count)
        {
            std::move(location + count, end, location);
        }
        else
        {
            std::move(end - count, end, location);
        }
        std::destroy(end - count, end);
        my_val.size -= count;
        return iterator(location);
    }
    /**
     * @brief Finds element within the array.
     * @param search
     * @return Index of the found element. INDEX_NONE otherwise.
     */
    NODISCARD size_type Find(const param_type search) const
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
    /**
     * @brief Finds element within the array.
     * @param predicate
     * @return Index of the found element. INDEX_NONE otherwise.
     */
    template<typename Predicate>
    NODISCARD size_type Find(const Predicate& predicate) const requires std::predicate<Predicate, const param_type>
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
    /**
     * @brief Finds last element within the array.
     * @param search
     * @return Index of the found element. INDEX_NONE otherwise.
     */
    NODISCARD size_type FindLast(const param_type search) const
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
    /**
     * @brief Finds last element within the array.
     * @param predicate
     * @return Index of the found element. INDEX_NONE otherwise.
     */
    template<typename Predicate>
    NODISCARD size_type FindLast(const Predicate& predicate) const requires std::predicate<Predicate, const param_type>
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
    /**
     * @brief Reserves memory such that the array can contain at least number elements.
     * @param capacity
     */
    void Reserve(size_type capacity)
    {
        auto&& my_val = GetVal();
        if (capacity > my_val.capacity)
        {
            size_type new_capacity = CalculateGrowth(capacity);
            if (my_val.capacity <= 0)
            {
                my_val.ptr = allocator_traits::allocate(GetAlloc(), new_capacity);
                my_val.capacity = new_capacity;
            }
            else
            {
                Reallocate(my_val, new_capacity);
            }
        }
    }
    /**
     * @brief Clear the array
     * @param reset_capacity Deallocate the remain capacity. Default is false.
     */
    void Clear(bool reset_capacity = false)
    {
        auto&& my_val = GetVal();
        if (my_val.size > 0)
        {
            std::destroy(my_val.ptr, my_val.ptr + my_val.size);
            my_val.size = 0;
        }

        if (reset_capacity && my_val.capacity > 0)
        {
            auto&& alloc = GetAlloc();
            allocator_traits::deallocate(alloc, my_val.ptr, my_val.capacity);
            my_val.ptr = nullptr;
            my_val.capacity = 0;
        }
    }
    /**
     * @brief Shrinks the array's used memory to smallest possible to store elements currently in it.
     */
    void ShrinkToFit()
    {
        auto&& my_val = GetVal();
        if (my_val.capacity > 0 && my_val.size < my_val.capacity)
        {
            Reallocate(my_val, my_val.size);
        }
    }

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

    template<std::forward_iterator InputIter, std::output_iterator<T> OutputIter>
    void MoveToUninitialized(InputIter first, InputIter last, OutputIter dest)
    {
        if constexpr (std::is_trivially_copyable<value_type>::value)
        {
            std::memmove(static_cast<void*>(IteratorToPointer(dest)), static_cast<const void*>(IteratorToPointer(first)), std::distance(first, last) * sizeof(value_type));
        }
        else
        {
            auto&& alloc = GetAlloc();
            InputIter it = first;
            while (it != last)
            {
                allocator_traits::construct(alloc, IteratorToPointer(dest), std::move(*it));
                std::advance(it, 1);
                std::advance(dest, 1);
            }
        }
    }

    template<std::forward_iterator InputIter, std::output_iterator<T> OutputIter>
    void CopyToUninitialized(InputIter first, InputIter last, OutputIter dest)
    {
        if constexpr (std::is_trivially_copyable<value_type>::value)
        {
            std::memmove(static_cast<void*>(IteratorToPointer(dest)), static_cast<const void*>(IteratorToPointer(first)), std::distance(first, last) * sizeof(value_type));
        }
        else
        {
            auto&& alloc = GetAlloc();
            InputIter it = first;
            while (it != last)
            {
                allocator_traits::construct(alloc, IteratorToPointer(dest), *it);
                std::advance(it, 1);
                std::advance(dest, 1);
            }
        }
    }

    template<typename Iter>
    iterator InsertCountedRange(const_iterator where, Iter first, size_type count, bool move_assign = false)
    {
        if (count <= 0)
        {
            return begin() + (where - cbegin());
        }

        auto&& my_val = GetVal();
        size_type unused_capacity = my_val.capacity - my_val.size;

        if (count <= unused_capacity)
        {
            const_iterator end = cend();
            size_type offset = where - cbegin();
            size_type move_count = end - where;
            pointer data = my_val.ptr;
            if (move_count > 0)
            {
                if (move_count <= count)
                {
                    // move to uninitialized location
                    MoveToUninitialized(where, end, data + offset + count);
                    std::destroy(where, end);
                }
                else
                {
                    MoveToUninitialized(end - count, end, data + my_val.size);
                    std::move_backward(where, end - count, this->end());
                    std::destroy(where, where + count);
                }
            }
            move_assign ?
            MoveToUninitialized(first, first + count, data + offset) :
            CopyToUninitialized(first, first + count, data + offset);
            my_val.size += count;
            return begin() + offset;;
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
            const_iterator begin = cbegin();
            const_iterator end = cend();
            size_type offset = where - begin;
            move_assign ?
            MoveToUninitialized(first, first + count, new_ptr + offset) :
            CopyToUninitialized(first, first + count, new_ptr + offset);
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
            return iterator(new_ptr + offset);
        }
    }

    template<std::ranges::forward_range RangeType>
    void Construct(const RangeType& range)
    {
        size_type size = std::ranges::distance(range);
        Reserve(math::Max(size, GetInitializeCapacity()));
        CopyToUninitialized(std::ranges::begin(range), std::ranges::end(range), end());
        GetVal().size = size;
    }

    void Construct(const_param_type value, size_type count)
    {
        Reserve(math::Max(count, GetInitializeCapacity()));
        if (count > 0)
        {
            auto&& my_val = GetVal();
            auto&& alloc = GetAlloc();
            for (int32 i = 0; i < count; ++i)
            {
                allocator_traits::construct(alloc, my_val.ptr + i, value);
            }

            my_val.size = count;
        }
    }

    void CopyAssignAllocator(const allocator_type& alloc)
    {
        if constexpr (allocator_traits::propagate_on_container_copy_assignment::value)
        {
            if (GetAlloc() != alloc)
            {
                Clear(true);
            }
            GetAlloc() = alloc;
        }
    }

    void MoveAssignAllocator(allocator_type& alloc) noexcept(std::is_nothrow_move_assignable_v<allocator_type>)
    {
        if constexpr (allocator_traits::propagate_on_container_move_assignment::value)
        {
            GetAlloc() = std::move(alloc);
        }
    }

    template<std::ranges::forward_range RangeType>
    void CopyAssign(const RangeType& range)
    {
        auto&& my_val = GetVal();
        size_type new_size = ConvertSize(std::ranges::distance(range));
        if (new_size > my_val.capacity)
        {
            Clear();
            Reallocate(my_val, CalculateGrowth(new_size));
            CopyToUninitialized(std::ranges::begin(range), std::ranges::end(range), my_val.ptr);
        }
        else
        {
            bool growing = false;
            auto&& first = std::ranges::begin(range);
            auto&& last = std::ranges::end(range);
            auto mid = last;
            if (new_size > my_val.size)
            {
                growing = true;
                mid = first;
                std::ranges::advance(mid, my_val.size);
            }

            pointer next = std::copy(first, mid, my_val.ptr);
            if (growing)
            {
                CopyToUninitialized(mid, last, next);
            }
            else
            {
                std::destroy(next, my_val.ptr + my_val.size);
            }
        }
        my_val.size = new_size;
    }

    template<std::ranges::forward_range RangeType>
    void MoveAssign(const RangeType& range) noexcept
    {
        auto&& my_val = GetVal();
        size_type new_size = ConvertSize(std::ranges::distance(range));
        if (new_size > my_val.capacity)
        {
            Clear();
            Reallocate(my_val, CalculateGrowth(new_size));
            MoveToUninitialized(std::ranges::begin(range), std::ranges::end(range), my_val.ptr);
        }
        else
        {
            bool growing = false;
            auto&& first = std::ranges::begin(range);
            auto&& last = std::ranges::end(range);
            auto mid = last;
            if (new_size > my_val.size)
            {
                growing = true;
                mid = first;
                std::ranges::advance(mid, my_val.size);
            }

            pointer next = std::move(first, mid, my_val.ptr);
            if (growing)
            {
                MoveToUninitialized(mid, last, next);
            }
            else
            {
                std::destroy(next, my_val.ptr + my_val.size);
            }
        }
        my_val.size = new_size;
    }

    void MoveAssign(Array& right, std::true_type) noexcept
    {
        if constexpr (GetAlloc() != right.GetAlloc())
        {
            CopyAssign(right);
        }
        else
        {
            MoveAssign(right, std::false_type());
        }
    }

    void MoveAssign(Array& right, std::false_type) noexcept
    {
        MoveAssignAllocator(right.GetAlloc());
        auto&& my_val = GetVal();
        auto&& right_val = right.GetVal();
        my_val.ptr = right_val.ptr;
        my_val.size = right_val.size;
        my_val.capacity = right_val.capacity;
        right_val.ptr = 0;
        right_val.size = right_val.capacity = 0;
    }

    constexpr size_type GetInitializeCapacity() const
    {
        return math::Min(math::Max(size_type(4), allocator_traits::get_initialize_size(pair_.First())), MaxSize());
    }

    size_type CalculateGrowth(size_type requested) const
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

    void Reallocate(val_type& val, size_type new_capacity)
    {
        if constexpr (std::is_trivially_copyable<value_type>::value)
        {
            val.ptr = allocator_traits::reallocate(GetAlloc(), val.ptr, val.capacity, new_capacity);
        }
        else
        {
            allocator_type& allocator = GetAlloc();
            pointer new_ptr = allocator_traits::allocate(allocator, new_capacity);
            pointer old_ptr = val.ptr;
            if (new_ptr != old_ptr)
            {
                MoveToUninitialized(old_ptr, old_ptr + val.size, new_ptr);
                std::destroy(old_ptr, old_ptr + val.size);
                allocator_traits::deallocate(GetAlloc(), old_ptr, val.capacity);
            }
            val.ptr = new_ptr;
        }

        val.capacity = new_capacity;
    }

    size_type AddUninitialized(size_type& increase_size)
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
                    my_val.capacity = new_capacity;
                }
                else
                {
                    Reallocate(my_val, new_capacity);
                }
            }
        }

        my_val.size = new_size;
        return old_size;
    }

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

template<typename T, uint32 N>
using InlineArray = Array<T, InlineAllocator<T, N>>;

template<typename T, uint32 N>
using FixedArray = Array<T, StackAllocator<T, N>>;

} // namespace atlas
