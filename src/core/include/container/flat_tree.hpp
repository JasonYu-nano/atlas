// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <algorithm>

#include "memory/allocator.hpp"
#include "container/array.hpp"
#include "utility/algorithm.hpp"

namespace atlas
{

namespace details
{

template<typename Value, typename KeyOfValue, typename Compare>
class FlatTreeValueCompare : private Compare
{
public:
    FlatTreeValueCompare() : Compare() {}
    explicit FlatTreeValueCompare(const Compare& predicate) : Compare(predicate) {}

    bool operator() (const Value& lhs, const Value& rhs) const
    {
        KeyOfValue key_extract;
        return Compare::operator()(key_extract(lhs), key_extract(rhs));
    }

    Compare& GetCompare() { return *this; }
    const Compare& GetCompare() const { return *this; }
};

}

template<typename ValueType, typename KeyOfValue, typename Compare, typename Allocator>
class FlatTree
{
public:
    using container_type            = Array<ValueType, Allocator>;
    using value_type                = typename container_type::value_type;
    using pointer                   = typename container_type::pointer;
    using const_pointer             = typename container_type::const_pointer;
    using reference                 = typename container_type::reference;
    using const_reference           = typename container_type::const_reference;
    using param_type                = typename container_type::param_type;
    using key_type                  = typename KeyOfValue::type;
    using key_param_type            = typename CallTraits<key_type>::param_type;
    using key_compare               = Compare;
    using allocator_type            = typename container_type::allocator_type;
    using allocator_traits          = typename container_type::allocator_traits;
    using size_type                 = typename container_type::size_type;
    using difference_type           = typename container_type::difference_type;
    using iterator                  = typename container_type::iterator;
    using const_iterator            = typename container_type::const_iterator;
    using reverse_iterator          = typename container_type::reverse_iterator;
    using const_reverse_iterator    = typename container_type::const_reverse_iterator;

private:
    using value_compare             = details::FlatTreeValueCompare<value_type, KeyOfValue, key_compare>;

public:
    /**
     * @brief Constructor.
     */
    explicit FlatTree() : pair_() {};
    /**
     * @brief Constructor, initialize key compare.
     * @param compare
     */
    explicit FlatTree(const key_compare& compare) : pair_(compare) {};
    /**
     * @brief Constructor, initialize allocator.
     * @param alloc
     */
    explicit FlatTree(const allocator_type& alloc) : pair_(ZeroThenVariadicArgs(), alloc) {};
    /**
     * @brief Constructor, initialize key compare and allocator.
     * @param compare
     * @param alloc
     */
    FlatTree(const key_compare& compare, const allocator_type& alloc) : pair_(OneThenVariadicArgs(), compare, alloc) {};
    /**
     * @brief Constructor, initialize with given capacity.
     * @param compare
     * @param capacity
     * @param alloc
     */
    FlatTree(const key_compare& compare, size_type capacity, const allocator_type& alloc)
        : pair_(OneThenVariadicArgs(), compare, capacity, alloc) {};
    /**
     * @brief Constructor from an initializer
     * @param compare
     * @param is_unique Skip duplicate elements
     * @param initializer
     * @param alloc
     */
    FlatTree(const key_compare& compare, bool is_unique, const std::initializer_list<value_type>& initializer, const allocator_type& alloc)
        : pair_(OneThenVariadicArgs(), compare, alloc)
    {
        is_unique ? InsertUnique(initializer) : InsertEqual(initializer);
    };
    /**
     * @brief Constructor from a range
     * @tparam RangeType
     * @param compare
     * @param is_unique Skip duplicate elements
     * @param range
     * @param alloc
     */
    template<std::ranges::bidirectional_range RangeType>
    FlatTree(const key_compare& compare, bool is_unique, const RangeType& range, const allocator_type& alloc)
        : pair_(OneThenVariadicArgs(), compare, alloc)
    {
        is_unique ? InsertUnique(range) : InsertEqual(range);
    }
    template<typename AnyCompare, typename AnyAllocator>
    explicit FlatTree(const FlatTree<value_type, KeyOfValue, AnyCompare, AnyAllocator>& right) : pair_(right.pair_) {};
    template<typename AnyCompare, typename AnyAllocator>
    explicit FlatTree(FlatTree<value_type, KeyOfValue, AnyCompare, AnyAllocator>&& right) noexcept : pair_(std::move(right.pair_)) {};

    template<typename AnyCompare, typename AnyAllocator>
    FlatTree& operator= (const FlatTree<value_type, KeyOfValue, AnyCompare, AnyAllocator>& right)
    {
        if (this != std::addressof(right))
        {
            GetValueCompare() = right.GetValueCompare();
            GetUnderlyingContainer() = right.GetUnderlyingContainer();
        }
        return *this;
    }

    template<typename AnyCompare, typename AnyAllocator>
    FlatTree& operator= (FlatTree<value_type, KeyOfValue, AnyCompare, AnyAllocator>&& right) noexcept
    {
        if (this != std::addressof(right))
        {
            GetValueCompare() = std::move(right.GetValueCompare());
            GetUnderlyingContainer() = std::move(right.GetUnderlyingContainer());
        }
        return *this;
    }

    /**
     * @brief Get number of elements in container.
     * @return
     */
    NODISCARD size_type Size() const { return GetUnderlyingContainer().Size(); }
    NODISCARD DO_NOT_USE_DIRECTLY size_type size() const { return Size(); }
    /**
     * @brief Get capacity of container.
     * @return
     */
    NODISCARD size_type Capacity() const { return GetUnderlyingContainer().Capacity(); }
    /**
     * @brief Inserts element only if there is no element with key equivalent to the key of that element.
     * @param value
     * @param already_in_tree Optional pointer to bool that will be set depending on whether element is already in tree.
     * @return Iterator to insert element.
     */
    iterator InsertUnique(const param_type value, bool* already_in_tree = nullptr)
    {
        iterator position;
        bool can_insert = FindPositionToInsert(KeyOfValue()(value), position);
        if (can_insert)
        {
            position = GetUnderlyingContainer().Insert(position, value);
        }
        if (already_in_tree)
        {
            *already_in_tree = !can_insert;
        }
        return position;
    }
    /**
     * @brief Inserts element only if there is no element with key equivalent to the key of that element.
     * @param value
     * @param already_in_tree Optional pointer to bool that will be set depending on whether element is already in tree.
     * @return Iterator to insert element.
     */
    iterator InsertUnique(value_type&& value, bool* already_in_tree = nullptr)
    {
        iterator position;
        bool can_insert = FindPositionToInsert(KeyOfValue()(value), position);
        if (can_insert)
        {
            position = GetUnderlyingContainer().Insert(position, std::move(value));
        }
        if (already_in_tree)
        {
            *already_in_tree = !can_insert;
        }
        return position;
    }
    /**
     * @brief Inserts each element from the range only if there is no element with key equivalent to the key of that element
     * @tparam RangeType
     * @param range
     */
    template<std::ranges::forward_range RangeType>
    void InsertUnique(const RangeType& range)
    {
        container_type& container = GetUnderlyingContainer();
        const value_compare& val_cmp = GetValueCompare();

        //Step 1: put new elements in the back
        const iterator it = container.Insert(container.cend(), range);

        //Step 2: sort them
        std::sort(it, container.end(), val_cmp);

        //Step 3: only left unique values from the back not already present in the original range
        iterator const e = algorithm::InplaceSetDifference(it, container.end(), container.begin(), it, val_cmp);

        container.RemoveAt(e, container.cend() - e);
        //it might be invalidated by erasing [e, seq.end) if e == it
        if (it != e)
        {
            //Step 4: merge both ranges
            std::inplace_merge(container.begin(), it, container.end(), val_cmp);
        }
    }
    /**
     * @brief Inserts element even if there is element with key equivalent to the key of that element.
     * @param value
     * @return Iterator to insert element.
     */
    iterator InsertEqual(const param_type value)
    {
        KeyOfValue key_extract;
        iterator position = std::ranges::upper_bound(begin(), end(), key_extract(value), GetKeyCompare(), key_extract);
        position = GetUnderlyingContainer().Insert(position, value);
        return position;
    }
    /**
     * @brief Inserts element even if there is element with key equivalent to the key of that element.
     * @param value
     * @return Iterator to insert element.
     */
    iterator InsertEqual(value_type&& value)
    {
        KeyOfValue key_extract;
        iterator position = std::ranges::upper_bound(begin(), end(), key_extract(value), GetKeyCompare(), key_extract);
        position = GetUnderlyingContainer().Insert(position, std::move(value));
        return position;
    }
    /**
     * @brief Inserts each element from the range even if there is element with key equivalent to the key of that element.
     * @tparam RangeType
     * @param range
     */
    template<std::ranges::forward_range RangeType>
    void InsertEqual(const RangeType& range)
    {
        container_type& container = GetUnderlyingContainer();
        const value_compare& val_cmp = GetValueCompare();

        //Step 1: put new elements in the back
        const iterator it = container.Insert(container.cend(), range);

        //Step 2: sort them
        std::sort(it, container.end(), val_cmp);
        //Step 3: merge both ranges
        std::inplace_merge(container.begin(), it, container.end(), val_cmp);
    }
    /**
     * @brief Finds element with key equivalent to the given key.
     * @param key
     * @return Iterator to found element. Iterator to the end otherwise.
     */
    iterator Find(const key_param_type key)
    {
        const key_compare& key_cmp = GetKeyCompare();
        KeyOfValue key_extract;
        iterator i = std::ranges::lower_bound(begin(), end(), key, key_cmp, key_extract);
        iterator end_it = end();
        if (i != end_it && key_cmp(key, key_extract(*i)))
        {
            i = end_it;
        }
        return i;
    }
    /**
     * @brief Finds element with key equivalent to the given key.
     * @param key
     * @return Iterator to found element. Iterator to the end otherwise.
     */
    const_iterator Find(const key_param_type key) const
    {
        const key_compare& key_cmp = GetKeyCompare();
        KeyOfValue key_extract;
        const_iterator i = std::ranges::lower_bound(begin(), end(), key, key_cmp, key_extract);
        const_iterator end_it = cend();
        if (i != end_it && key_cmp(key, key_extract(*i)))
        {
            i = end_it;
        }
        return i;
    }
    /**
     * @brief Get number of elements with key equivalent to the given key.
     * @param key
     * @return Number of elements with key equivalent to the given key.
     */
    NODISCARD size_type Count(const key_param_type key) const
    {
        const key_compare& key_cmp = GetKeyCompare();
        KeyOfValue key_extract;
        const_iterator lower = std::ranges::lower_bound(begin(), end(), key, key_cmp, key_extract);
        const_iterator end_it = cend();
        if (lower == end_it || key_cmp(key, key_extract(*lower)))
        {
            return 0;
        }
        const_iterator upper = std::ranges::upper_bound(begin(), end(), key, key_cmp, key_extract);
        return upper - lower;
    }
    /**
     * @brief Removes element at given position.
     * @param where
     * @return Iterator to removed element.
     */
    iterator Remove(const_iterator where)
    {
        return GetUnderlyingContainer().RemoveAt(where);
    }
    /**
     * @brief Removes elements with key equivalent to the given key.
     * @param key
     * @return Number of removed element.
     */
    size_type Remove(const key_param_type key)
    {
        const key_compare& key_cmp = GetKeyCompare();
        KeyOfValue key_extract;
        const_iterator lower = std::ranges::lower_bound(begin(), end(), key, key_cmp, key_extract);
        const_iterator end_it = cend();
        if (lower == end_it || key_cmp(key, key_extract(*lower)))
        {
            return 0;
        }

        const_iterator upper = std::ranges::upper_bound(begin(), end(), key, key_cmp, key_extract);
        size_type count = upper - lower;
        GetUnderlyingContainer().RemoveAt(lower, count);
        return count;
    }
    /**
     * @brief Removes element with key equivalent to the given key.
     * @param key
     * @return Iterator to removed element. Iterator to the end otherwise.
     */
    size_type RemoveUnique(const key_param_type key)
    {
        const_iterator i = Find(key);
        size_type ret = i != this->cend() ? 1 : 0;
        if (ret != 0)
        {
            Remove(i);
        }
        return ret;
    }
    /**
     * @brief Clear all elements.
     * @param reset_capacity
     */
    void Clear(bool reset_capacity = false)
    {
        GetUnderlyingContainer().Clear(reset_capacity);
    }
    /**
     * @brief Reserves memory such that the container can contain at least number elements.
     * @param capacity
     */
    void Reserve(size_type capacity)
    {
        GetUnderlyingContainer().Reserve(capacity);
    }
    /**
     * @brief Shrinks the container's used memory to smallest possible to store elements currently in it.
     */
    void ShrinkToFit()
    {
        GetUnderlyingContainer().ShrinkToFit();
    }

    NODISCARD iterator begin() { return GetUnderlyingContainer().begin(); }
    NODISCARD const_iterator begin() const{ return GetUnderlyingContainer().begin(); }
    NODISCARD iterator end() { return GetUnderlyingContainer().end(); }
    NODISCARD const_iterator end() const { return GetUnderlyingContainer().end(); }

    NODISCARD reverse_iterator rbegin() { return GetUnderlyingContainer().rbegin(); }
    NODISCARD const_reverse_iterator rbegin() const { return GetUnderlyingContainer().rbegin(); }
    NODISCARD reverse_iterator rend() { return GetUnderlyingContainer().rend(); }
    NODISCARD const_reverse_iterator rend() const { return GetUnderlyingContainer().rend(); }

    NODISCARD const_iterator cbegin() const { return GetUnderlyingContainer().cbegin(); }
    NODISCARD const_iterator cend() const { return GetUnderlyingContainer().cend(); }
    NODISCARD const_reverse_iterator crbegin() const { return GetUnderlyingContainer().crbegin(); }
    NODISCARD const_reverse_iterator crend() const { return GetUnderlyingContainer().crend(); }

private:
    const value_compare& GetValueCompare() const
    {
        return pair_.First();
    }

    const key_compare& GetKeyCompare() const
    {
        return GetValueCompare().GetCompare();
    }

    container_type& GetUnderlyingContainer()
    {
        return pair_.Second();
    }

    const container_type& GetUnderlyingContainer() const
    {
        return pair_.Second();
    }

    bool FindPositionToInsert(const key_param_type key, const_iterator& position)
    {
        return FindPositionToInsert(cbegin(), cend(), key, position);
    }

    bool FindPositionToInsert(const_iterator begin, const_iterator end, const key_param_type key, const_iterator& position)
    {
        const key_compare& key_cmp = GetKeyCompare();
        KeyOfValue key_extract;
        position = std::ranges::lower_bound(begin, end, key, key_cmp, key_extract);
        return position == end || key_cmp(key, key_extract(*position));
    }

    CompressionPair<value_compare, container_type> pair_;
};

}
