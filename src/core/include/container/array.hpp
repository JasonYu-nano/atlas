// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <type_traits>
#if PLATFORM_APPLE
#include <__bit_reference>
#endif

#include "memory/allocator.hpp"
#include "core_macro.hpp"
#include "misc/iterator.hpp"
#include "utility/compression_pair.hpp"


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
    using value_type                = T;
    using allocator_type            = typename AllocatorRebind<Allocator, T>::type;
    using allocator_traits          = AllocatorTraits<allocator_type>;
    using size_type                 = typename allocator_traits::size_type;
    using difference_type           = typename allocator_traits::difference_type;
    using pointer                   = value_type*;
    using const_pointer             = const value_type*;
    using reference                 = typename CallTraits<value_type>::reference;
    using const_reference           = typename CallTraits<value_type>::const_reference;
    using param_type                = typename CallTraits<value_type>::param_type;
    using const_param_type          = const param_type;
    using iterator                  = PointerIterator<value_type>;
    using const_iterator            = ConstPointerIterator<value_type>;
    using reverse_iterator          = std::reverse_iterator<iterator>;
    using const_reverse_iterator    = std::reverse_iterator<const_iterator>;

private:
    using val_type                  = details::ArrayVal<value_type, size_type>;

public:
    /**
     * @brief Constructor.
     * @param alloc
     */
    explicit Array(const allocator_type& alloc = allocator_type()) : pair_(alloc)
    {
        reserve(get_initialize_capacity());
    }
    /**
     * @brief Constructor, initialize with given capacity.
     * @param capacity
     * @param alloc
     */
    explicit Array(size_type capacity, const allocator_type& alloc = allocator_type()) : pair_(alloc)
    {
        reserve(math::Max(capacity, get_initialize_capacity()));
    }
    /**
     * @brief Constructor from an initializer
     * @param initializer
     * @param alloc
     */
    Array(const std::initializer_list<value_type>& initializer, const allocator_type& alloc = allocator_type()) : pair_(alloc)
    {
        construct(initializer);
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
        construct(range);
    }
    /**
     * @brief Constructor from given number elements.
     * @param value
     * @param count
     * @param alloc
     */
    Array(const_param_type value, size_type count, const allocator_type& alloc = allocator_type()) : pair_(alloc)
    {
        construct(value, count);
    }
    /**
     * @brief Copy constructor.
     * @param right
     */
    Array(const Array& right) : pair_(allocator_traits::select_on_container_copy_construction(right.get_alloc()))
    {
        construct(right);
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
        construct(right);
    }
    /**
     * @brief Move constructor.
     * @param right
     */
    Array(Array&& right) noexcept
        : pair_(OneThenVariadicArgs()
        , std::move(right.get_alloc())
        , std::exchange(right.get_val().size, 0)
        , std::exchange(right.get_val().capacity, 0)
        , std::exchange(right.get_val().ptr, nullptr))
    {}
    /**
     * @brief Move constructs from other allocator type array.
     * @tparam OtherAllocator
     * @param right
     * @param alloc
     */
    template<typename OtherAllocator>
    Array(Array<value_type, OtherAllocator>&& right, const allocator_type& alloc = allocator_type()) noexcept : pair_(alloc)
    {
        move_assign(right);
        right.get_val().size = 0;
    }

    ~Array() noexcept
    {
        clear(true);
    }

    Array& operator= (const Array& right)
    {
        if (this != std::addressof(right))
        {
            copy_assign_allocator(right.get_alloc());
            copy_assign(right);
        }
        return *this;
    }

    Array& operator= (Array&& right) noexcept
    {
        if (this != std::addressof(right))
        {
            move_assign(right, std::integral_constant<bool,
                allocator_traits::propagate_on_container_move_assignment::value>());
        }
        return *this;
    }

    template<typename OtherAllocator>
    Array& operator= (const Array<value_type, OtherAllocator>& right)
    {
        copy_assign(right);
        return *this;
    }

    template<typename OtherAllocator>
    Array& operator= (Array<value_type, OtherAllocator>&& right)
    {
        move_assign(right);
        right.get_val().size = 0;
        return *this;
    }
    /**
     * @brief Get reference to element at given index.
     * @param index
     * @return
     */
    NODISCARD reference operator[] (size_type index)
    {
        ASSERT(is_valid_index(index));
        return data()[index];
    }
    /**
     * @brief Get const reference to element at given index.
     * @param index
     * @return
     */
    NODISCARD const_reference operator[] (size_type index) const
    {
        ASSERT(is_valid_index(index));
        return data()[index];
    }
    /**
     * @brief Get number of elements in array.
     * @return
     */
    NODISCARD size_type size() const { return get_val().size; }
    /**
     * @brief Get maximum number of elements in array.
     * @return
     */
    NODISCARD constexpr size_type max_size() const { return allocator_traits::max_size(get_alloc()); }
    /**
     * @brief Returns pointer to the first array element.
     * @return
     */
    NODISCARD pointer data() { return get_val().ptr; }
    /**
     * @brief Returns const pointer to the first array element.
     * @return
     */
    NODISCARD const_pointer data() const { return get_val().ptr; }
    /**
     * @brief Get last index of array.
     * @return
     */
    NODISCARD size_type last_index() const
    {
        ASSERT(size() > 0);
        return size() - 1;
    }
    /**
     * @brief Get n-th last element from the array.
     * @return
     */
    NODISCARD reference last(size_type index_from_end = 0)
    {
        return data()[last_index() - index_from_end];
    }
    /**
     * @brief Get n-th last element from the array.
     * @return
     */
    NODISCARD const_reference last(size_type index_from_end = 0) const
    {
        return data()[last_index() - index_from_end];
    }
    /**
     * @brief Get capacity of array
     * @return
     */
    NODISCARD size_type capacity() const { return get_val().capacity; }
    /**
     * @brief Returns true if the array is empty and contains no elements.
     * @return
     */
    NODISCARD bool is_empty() const { return size() <= 0; }
    /**
     * @brief Tests if index is valid.
     * @return
     */
    NODISCARD bool is_valid_index(size_type index) const { return index >= 0 && index < size(); }
    /**
     * @brief Copies a new element to the end of the array, possibly reallocating the whole array to fit.
     * @param elem
     * @return Index to the new element
     */
    size_type add(const param_type elem) { return emplace(elem); }
    /**
     * @brief Moves a new element to the end of the array, possibly reallocating the whole array to fit.
     * @param elem
     * @return Index to the new element
     */
    size_type add(value_type&& elem) { return emplace(elem); }
    /**
     * @brief Copies unique element to array if it doesn't exist.
     * @param elem
     * @return Index to the element
     */
    size_type add_unique(const param_type elem)
    {
        size_type index = find(elem);
        return index != INDEX_NONE ? index : emplace(elem);
    }
    /**
     * @brief Moves unique element to array if it doesn't exist.
     * @param elem
     * @return Index to the element
     */
    size_type add_unique(value_type&& elem)
    {
        size_type index = find(elem);
        return index != INDEX_NONE ? index : emplace(std::forward<value_type>(elem));
    }
    /**
     * @brief Constructs a new item at the end of the array, possibly reallocating the whole array to fit.
     * @tparam Args
     * @param args
     * @return Index to the new element
     */
    template<typename... Args>
    size_type emplace(Args&&... args)
    {
        size_type increase = 1;
        size_type index = add_uninitialized(increase);
        if (index != INDEX_NONE)
        {
            new (data() + index) value_type(std::forward<Args>(args)...);
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
    size_type append(const RangeType& others)
    {
        size_type increase_size = convert_size(std::ranges::distance(others));
        size_type index = add_uninitialized(increase_size);
        if constexpr (std::is_trivially_copyable_v<value_type>)
        {
            std::memmove(data() + index, IteratorToPointer(std::ranges::begin(others)), increase_size * sizeof(value_type));
        }
        else
        {
            pointer ptr = data() + index;
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
    size_type append(Array<value_type, AllocatorType>&& others)
    {
        size_type increase_size = others.size();
        size_type index = add_uninitialized(increase_size);
        if constexpr (std::is_trivially_copyable_v<value_type>)
        {
            std::memmove(data() + index, others.data(), increase_size * sizeof(value_type));
        }
        else
        {
            pointer ptr = data() + index;
            const_pointer source = others.data();
            for (size_type i = 0; i < increase_size; ++i)
            {
                new (ptr + i) value_type(std::move(source[i]));
            }
        }
        auto&& other_val = others.get_val();
        other_val.size = 0;

        return index;
    }
    /**
     * @brief Inserts given element into the array at given position.
     * @param where
     * @param elem
     */
    iterator insert(const_iterator where, const param_type elem)
    {
        return insert_counted_range(where, &elem, 1);
    }
    /**
     * @brief Inserts given element into the array at given position.
     * @param where
     * @param elem
     */
    iterator insert(const_iterator where, value_type&& elem)
    {
        return insert_counted_range(where, &elem, 1, true);
    }
    /**
     * @brief Inserts given elements into the array at given position.
     * @param where
     * @param elems
     */
    template<std::ranges::range RangeType>
    iterator insert(const_iterator where, const RangeType& elems)
    {
        if constexpr (std::ranges::sized_range<RangeType> || std::ranges::forward_range<RangeType>)
        {
            return insert_counted_range(where, std::ranges::begin(elems), std::ranges::distance(elems));
        }
        return begin() + (where - cbegin());
    }
    /**
     * @brief Inserts other array into the array at given position.
     * @param where
     * @param elems
     */
    template<typename AllocatorType>
    iterator insert(const_iterator where, Array<value_type, AllocatorType>&& elems)
    {
        return insert_counted_range(where, elems.begin(), elems.size(), true);
    }
    /**
     * @brief Removes first matched element in the array.
     * @param elem
     * @return Index of removed element
     */
    size_type remove(const param_type elem)
    {
        size_type index = find(elem);
        if (index != INDEX_NONE)
        {
            remove_at(cbegin() + index);
        }
        return index;
    }
    /**
     * @brief Removes first matched element in the array.
     * @param predicate
     * @return Index of removed element
     */
    template<typename Predicate>
    size_type remove(const Predicate& predicate) requires std::predicate<Predicate, const param_type>
    {
        size_type index = find(predicate);
        if (index != INDEX_NONE)
        {
            remove_at(cbegin() + index);
        }
        return index;
    }
    /**
     * @brief Removes first matched element in the array. It's faster than Remove but breaks the order
     * @param elem
     * @return Index of removed element
     */
    size_type remove_swap(const param_type elem)
    {
        size_type index = find(elem);
        if (index != INDEX_NONE)
        {
            remove_at_swap(cbegin() + index);
        }
        return index;
    }
    /**
     * @brief Removes first matched element in the array. It's faster than Remove but breaks the order
     * @param predicate
     * @return Index of removed element
     */
    template<typename Predicate>
    size_type remove_swap(const Predicate& predicate) requires std::predicate<Predicate, const param_type>
    {
        size_type index = find(predicate);
        if (index != INDEX_NONE)
        {
            remove_at_swap(cbegin() + index);
        }
        return index;
    }
    /**
     * @brief Removes all instances in the array that matched element.
     * @param elem
     * @return Numbers of removed elements
     */
    size_type remove_all(const param_type elem)
    {
        return remove_all([&elem](const param_type value) { return value == elem; });
    }
    /**
     * @brief Removes all instances in the array that matched element.
     * @param predicate
     * @return Numbers of removed elements
     */
    template<typename Predicate>
    size_type remove_all(const Predicate& predicate) requires std::predicate<Predicate, const param_type>
    {
        auto&& my_val = get_val();
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
    size_type remove_all_swap(const param_type elem)
    {
        return remove_all_swap([&elem](const param_type value) { return value == elem; });
    }
    /**
     * @brief Removes all instances in the array that matched element. It's faster than RemoveAll but breaks the order
     * @param predicate
     * @return Numbers of removed elements
     */
    template<typename Predicate>
    size_type remove_all_swap(const Predicate& predicate) requires std::predicate<Predicate, const param_type>
    {
        size_type num_of_matches = 0;
        if (size() > 0)
        {
            const_iterator read = cbegin();
            const_iterator last = cend();

            while (read < last)
            {
                if (predicate(*read))
                {
                    remove_at_swap(read);
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
    iterator remove_at(size_type where)
    {
        return remove_at(begin() + where, 1);
    }
    /**
     * @brief Removes multiple elements at given position.
     * @param where
     * @param count
     * @return Next element iterator
     */
    iterator remove_at(size_type where, size_type count)
    {
        return remove_at(begin() + where, count);
    }
    /**
     * @brief Removes element at given position.
     * @param where
     * @return Next element iterator
     */
    iterator remove_at(const_iterator where)
    {
        return remove_at(where, 1);
    }
    /**
     * @brief Removes multiple elements at given position.
     * @param where
     * @param count
     * @return Next element iterator
     */
    iterator remove_at(const_iterator where, size_type count)
    {
        if (count <= 0)
        {
            return iterator(const_cast<value_type*>(IteratorToPointer(where)));
        }
        ASSERT(cbegin() <= where && where < cend() && where + count <= cend());
        auto&& my_val = get_val();
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
    iterator remove_at_swap(size_type where)
    {
        return remove_at_swap(begin() + where, 1);
    }
    /**
     * @brief Removes multiple elements at given position. It's faster than RemoveAt but breaks the order
     * @param where
     * @param count
     * @return Next element iterator
     */
    iterator remove_at_swap(size_type where, size_type count)
    {
        return remove_at_swap(begin() + where, count);
    }
    /**
     * @brief Removes element at given position. It's faster than RemoveAt but breaks the order
     * @param where
     * @return Next element iterator
     */
    iterator remove_at_swap(const_iterator where)
    {
        return remove_at_swap(where, 1);
    }
    /**
     * @brief Removes multiple elements at given position. It's faster than RemoveAt but breaks the order
     * @param where
     * @param count
     * @return Next element iterator
     */
    iterator remove_at_swap(const_iterator where, size_type count)
    {
        ASSERT(cbegin() <= where && where < cend() && where + count <= cend());
        auto&& my_val = get_val();
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
    NODISCARD size_type find(const param_type search) const
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
    NODISCARD size_type find(const Predicate& predicate) const requires std::predicate<Predicate, const param_type>
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
    NODISCARD size_type find_last(const param_type search) const
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
    NODISCARD size_type find_last(const Predicate& predicate) const requires std::predicate<Predicate, const param_type>
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
    void reserve(size_type capacity)
    {
        auto&& my_val = get_val();
        if (capacity > my_val.capacity)
        {
            size_type new_capacity = calculate_growth(capacity);
            if (my_val.capacity <= 0)
            {
                my_val.ptr = allocator_traits::allocate(get_alloc(), new_capacity);
                my_val.capacity = new_capacity;
            }
            else
            {
                reallocate(my_val, new_capacity);
            }
        }
    }

    /**
     * @brief Resizes the container to contain count elements.
     * If the current size is greater than count, the container is reduced to its first count elements.
     * If the current size is less than count, additional default-inserted elements are appended.
     * @param count
     */
    void resize(size_type count)
    {
        resize(count, value_type{});
    }
    /**
     * @brief Resizes the container to contain count elements.
     * If the current size is greater than count, the container is reduced to its first count elements.
     * If the current size is less than count, additional copies of value are appended.
     * @param count
     * @param value
     */
    void resize(size_type count, const param_type value)
    {
        if (count < 0)
        {
            return;
        }

        auto&& my_val = get_val();
        size_type old_size = my_val.size;
        if (count == old_size)
        {
            return;
        }

        if (count < old_size)
        {
            // trim
            pointer data = my_val.ptr;
            pointer end = data + old_size;
            std::destroy(data + count, end);
        }
        else
        {
            if (count > my_val.capacity)
            {
                size_type new_capacity = calculate_growth(count);
                reallocate(my_val, new_capacity);
            }

            auto&& alloc = get_alloc();
            for (size_type i = old_size; i < count; ++i)
            {
                allocator_traits::construct(alloc, my_val.ptr + i, value);
            }
        }

        my_val.size = count;
    }
    /**
     * @brief Clear the array
     * @param reset_capacity Deallocate the remain capacity. Default is false.
     */
    void clear(bool reset_capacity = false)
    {
        auto&& my_val = get_val();
        if (my_val.size > 0)
        {
            std::destroy(my_val.ptr, my_val.ptr + my_val.size);
            my_val.size = 0;
        }

        if (reset_capacity && my_val.capacity > 0)
        {
            auto&& alloc = get_alloc();
            allocator_traits::deallocate(alloc, my_val.ptr, my_val.capacity);
            my_val.ptr = nullptr;
            my_val.capacity = 0;
        }
    }
    /**
     * @brief Shrinks the array's used memory to smallest possible to store elements currently in it.
     */
    void shrink_to_fit()
    {
        auto&& my_val = get_val();
        if (my_val.capacity > 0 && my_val.capacity > my_val.size)
        {
            reallocate(my_val, my_val.size);
        }
    }

    NODISCARD iterator begin() { return iterator(data()); }
    NODISCARD const_iterator begin() const{ return const_iterator(data()); }
    NODISCARD iterator end() { return iterator(data() + size()); }
    NODISCARD const_iterator end() const { return const_iterator(data() + size()); }

    NODISCARD reverse_iterator rbegin() { return reverse_iterator(end()); }
    NODISCARD const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    NODISCARD reverse_iterator rend() { return reverse_iterator(begin()); }
    NODISCARD const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

    NODISCARD const_iterator cbegin() const { return begin(); }
    NODISCARD const_iterator cend() const { return end(); }
    NODISCARD const_reverse_iterator crbegin() const { return const_reverse_iterator(end()); }
    NODISCARD const_reverse_iterator crend() const { return const_reverse_iterator(begin()); }

    val_type& get_val() { return pair_.Second(); }
    const val_type& get_val() const { return pair_.Second(); }
private:
    allocator_type& get_alloc() { return pair_.First(); }
    const allocator_type& get_alloc() const { return pair_.First(); }

    template<std::forward_iterator InputIter, std::output_iterator<T> OutputIter>
    void move_to_uninitialized(InputIter first, InputIter last, OutputIter dest)
    {
        if constexpr (std::is_trivially_copyable_v<value_type>)
        {
            std::memmove(static_cast<void*>(IteratorToPointer(dest)), static_cast<const void*>(IteratorToPointer(first)), std::distance(first, last) * sizeof(value_type));
        }
        else
        {
            auto&& alloc = get_alloc();
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
    void copy_to_uninitialized(InputIter first, InputIter last, OutputIter dest)
    {
        if constexpr (std::is_trivially_copyable_v<value_type>)
        {
            std::memmove(static_cast<void*>(IteratorToPointer(dest)), static_cast<const void*>(IteratorToPointer(first)), std::distance(first, last) * sizeof(value_type));
        }
        else
        {
            auto&& alloc = get_alloc();
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
    iterator insert_counted_range(const_iterator where, Iter first, size_type count, bool move_assign = false)
    {
        if (count <= 0)
        {
            return begin() + (where - cbegin());
        }

        auto&& my_val = get_val();
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
                    move_to_uninitialized(where, end, data + offset + count);
                    std::destroy(where, end);
                }
                else
                {
                    move_to_uninitialized(end - count, end, data + my_val.size);
                    std::move_backward(where, end - count, this->end());
                    std::destroy(where, where + count);
                }
            }
            move_assign ?
            move_to_uninitialized(first, first + count, data + offset) :
            copy_to_uninitialized(first, first + count, data + offset);
            my_val.size += count;
            return begin() + offset;;
        }
        else
        {
            if (count > max_size() - my_val.size)
            {
                ASSERT(0);
                count = max_size() - my_val.size;
            }

            size_type new_size = my_val.size + count;
            size_type new_capacity = calculate_growth(new_size);
            auto&& alloc = get_alloc();

            pointer new_ptr = allocator_traits::allocate(alloc, new_capacity);
            const_iterator begin = cbegin();
            const_iterator end = cend();
            size_type offset = where - begin;
            move_assign ?
            move_to_uninitialized(first, first + count, new_ptr + offset) :
            copy_to_uninitialized(first, first + count, new_ptr + offset);
            if (where == end)
            {
                move_to_uninitialized(begin, end, new_ptr);
            }
            else
            {
                move_to_uninitialized(begin, where, new_ptr);
                move_to_uninitialized(where, end, new_ptr + offset + count);
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
    void construct(const RangeType& range)
    {
        size_type size = std::ranges::distance(range);
        reserve(math::Max(size, get_initialize_capacity()));
        copy_to_uninitialized(std::ranges::begin(range), std::ranges::end(range), end());
        get_val().size = size;
    }

    void construct(const_param_type value, size_type count)
    {
        reserve(math::Max(count, get_initialize_capacity()));
        if (count > 0)
        {
            auto&& my_val = get_val();
            auto&& alloc = get_alloc();
            for (int32 i = 0; i < count; ++i)
            {
                allocator_traits::construct(alloc, my_val.ptr + i, value);
            }

            my_val.size = count;
        }
    }

    void copy_assign_allocator(const allocator_type& alloc)
    {
        if constexpr (allocator_traits::propagate_on_container_copy_assignment::value)
        {
            if (get_alloc() != alloc)
            {
                clear(true);
            }
            get_alloc() = alloc;
        }
    }

    void move_assign_allocator(allocator_type& alloc) noexcept(std::is_nothrow_move_assignable_v<allocator_type>)
    {
        if constexpr (allocator_traits::propagate_on_container_move_assignment::value)
        {
            get_alloc() = std::move(alloc);
        }
    }

    template<std::ranges::forward_range RangeType>
    void copy_assign(const RangeType& range)
    {
        auto&& my_val = get_val();
        size_type new_size = convert_size(std::ranges::distance(range));
        if (new_size > my_val.capacity)
        {
            clear();
            reallocate(my_val, calculate_growth(new_size));
            copy_to_uninitialized(std::ranges::begin(range), std::ranges::end(range), my_val.ptr);
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
                copy_to_uninitialized(mid, last, next);
            }
            else
            {
                std::destroy(next, my_val.ptr + my_val.size);
            }
        }
        my_val.size = new_size;
    }

    template<std::ranges::forward_range RangeType>
    void move_assign(const RangeType& range) noexcept
    {
        auto&& my_val = get_val();
        size_type new_size = convert_size(std::ranges::distance(range));
        if (new_size == 0)
        {
            std::destroy(my_val.ptr, my_val.ptr + my_val.size);
        }
        else if (new_size > my_val.capacity)
        {
            clear();
            reallocate(my_val, calculate_growth(new_size));
            move_to_uninitialized(std::ranges::begin(range), std::ranges::end(range), my_val.ptr);
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
                move_to_uninitialized(mid, last, next);
            }
            else
            {
                std::destroy(next, my_val.ptr + my_val.size);
            }
        }
        my_val.size = new_size;
    }

    void move_assign(Array& right, std::true_type) noexcept
    {
        if constexpr (get_alloc() != right.get_alloc())
        {
            copy_assign(right);
        }
        else
        {
            MoveAssign(right, std::false_type());
        }
    }

    void move_assign(Array& right, std::false_type) noexcept
    {
        move_assign_allocator(right.get_alloc());
        auto&& my_val = get_val();
        auto&& right_val = right.get_val();
        my_val.ptr = right_val.ptr;
        my_val.size = right_val.size;
        my_val.capacity = right_val.capacity;
        right_val.ptr = 0;
        right_val.size = right_val.capacity = 0;
    }

    constexpr size_type get_initialize_capacity() const
    {
        return math::Min(math::Max(size_type(4), allocator_traits::get_initialize_size(pair_.First())), max_size());
    }

    size_type calculate_growth(size_type requested) const
    {
        if (requested > max_size())
        {
            return max_size();
        }

        size_type old = get_val().capacity;
        if (old > max_size() - old)
        {
            return max_size();
        }

        return math::Max(requested, 2 * old);
    }

    void reallocate(val_type& val, size_type new_capacity)
    {
        if constexpr (std::is_trivially_copyable_v<value_type>)
        {
            val.ptr = allocator_traits::reallocate(get_alloc(), val.ptr, val.capacity, new_capacity);
        }
        else
        {
            allocator_type& allocator = get_alloc();
            pointer new_ptr = allocator_traits::allocate(allocator, new_capacity);
            pointer old_ptr = val.ptr;
            if (old_ptr && new_ptr != old_ptr)
            {
                move_to_uninitialized(old_ptr, old_ptr + val.size, new_ptr);
                std::destroy(old_ptr, old_ptr + val.size);
                allocator_traits::deallocate(get_alloc(), old_ptr, val.capacity);
            }
            val.ptr = new_ptr;
        }

        val.capacity = new_capacity;
    }

    size_type add_uninitialized(size_type& increase_size)
    {
        auto&& my_val = get_val();
        size_type old_size = my_val.size;
        if (max_size() - old_size < increase_size)
        {
            ASSERT(0);
            increase_size = max_size() - old_size;
        }

        if (increase_size <= 0)
        {
            return static_cast<size_type>(INDEX_NONE);
        }

        size_type new_size = old_size + increase_size;
        if (new_size > my_val.capacity)
        {
            size_type new_capacity = calculate_growth(new_size);
            if (new_capacity > my_val.capacity)
            {
                auto&& alloc = get_alloc();
                if (my_val.capacity <= 0)
                {
                    my_val.ptr = allocator_traits::allocate(alloc, new_capacity);
                    my_val.capacity = new_capacity;
                }
                else
                {
                    reallocate(my_val, new_capacity);
                }
            }
        }

        my_val.size = new_size;
        return old_size;
    }

    template<std::integral SizeType>
    constexpr size_type convert_size(SizeType size)
    {
        ASSERT(size <= max_size());
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
