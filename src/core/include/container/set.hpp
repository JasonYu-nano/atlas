// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "container/flat_tree.hpp"

namespace atlas
{

namespace details
{

template <class T>
concept LessComparable = requires(const std::remove_reference_t<T>& t1, const std::remove_reference_t<T>& t2)
{
    { t1 <  t2 } -> std::same_as<bool>;
};

template<typename T>
struct SetKeyOfValue
{
    using type = T;

    const type& operator()(const type& t) const
    {
        return t;
    }
};
}

template<details::LessComparable T, typename Allocator = StandardAllocator<size_t>>
class Set : private FlatTree<T, details::SetKeyOfValue<T>, std::less<T>, Allocator>
{
    using base                      = FlatTree<T, details::SetKeyOfValue<T>, std::less<T>, Allocator>;
    using underlying_container_type = typename base::container_type;
    using key_compare               = typename base::key_compare;

public:
    using value_type                = typename base::value_type;
    using size_type                 = typename base::size_type;
    using param_type                = typename base::param_type;
    using allocator_type            = typename base::allocator_type;
    using iterator                  = typename base::iterator;
    using const_iterator            = typename base::const_iterator;
    using reverse_iterator          = typename base::reverse_iterator;
    using const_reverse_iterator    = typename base::const_reverse_iterator;

    using base::begin;
    using base::end;
    using base::cbegin;
    using base::cend;
    using base::rbegin;
    using base::rend;
    using base::crbegin;
    using base::crend;

    using base::Size;
    using base::size;
    using base::Capacity;
    using base::Reserve;
    using base::Clear;
    using base::ShrinkToFit;
    /**
     * @brief Constructor, initialize allocator.
     * @param alloc
     */
    explicit Set(const allocator_type& alloc = allocator_type()) : base(alloc) {};
    /**
     * @brief Constructor, initialize with given capacity.
     * @param capacity
     * @param alloc
     */
    explicit Set(size_type capacity, const allocator_type& alloc = allocator_type())
        : base(key_compare(), capacity, alloc) {};
    /**
     * @brief Constructor from an initializer
     * @param initializer
     * @param alloc
     */
    Set(const std::initializer_list<value_type >& initializer, const allocator_type& alloc = allocator_type())
        : base(key_compare(), true, initializer, alloc) {}
    /**
     * @brief Constructor from a range
     * @tparam RangeType
     * @param range
     * @param alloc
     */
    template<std::ranges::bidirectional_range RangeType>
    explicit Set(const RangeType& range, const allocator_type& alloc = allocator_type())
        : base(key_compare(), true, range, alloc) {}

    template<typename AnyAllocator>
    Set& operator= (const Set<value_type, AnyAllocator>& right)
    {
        base::operator=(right);
        return *this;
    }

    template<typename AnyAllocator>
    Set& operator= (Set<value_type, AnyAllocator>&& right)
    {
        base::operator=(std::forward<Set<value_type, AnyAllocator>>(right));
        return *this;
    }
    /**
     * @brief Inserts element only if there is no element equivalently.
     * @param value
     * @param already_in_set Optional pointer to bool that will be set depending on whether element is already in set.
     * @return Iterator to insert element.
     */
    iterator Insert(const param_type value, bool* already_in_set = nullptr)
    {
        return base::InsertUnique(value, already_in_set);
    }
    /**
     * @brief Inserts element only if there is no element equivalently.
     * @param value
     * @param already_in_set Optional pointer to bool that will be set depending on whether element is already in set.
     * @return Iterator to insert element.
     */
    iterator Insert(value_type&& value, bool* already_in_set = nullptr)
    {
        return base::InsertUnique(std::forward<value_type>(value), already_in_set);
    }
    /**
     * @brief Inserts each element from the range only if there is no element equivalently.
     * @tparam RangeType
     * @param range
     */
    template<std::ranges::forward_range RangeType>
    void Insert(const RangeType& range)
    {
        base::InsertUnique(range);
    }
    /**
     * @brief Returns whether set contains equivalently element.
     * @param value
     * @return
     */
    NODISCARD bool Contains(const param_type value) const
    {
        return base::Find(value) != end();
    }
    /**
     * @brief Finds element equivalent to the given one.
     * @param value
     * @return Iterator to found element. Iterator to the end otherwise.
     */
    NODISCARD iterator Find(const param_type value)
    {
        return base::Find(value);
    }
    /**
     * @brief Finds element equivalent to the given one.
     * @param value
     * @return Iterator to found element. Iterator to the end otherwise.
     */
    NODISCARD const_iterator Find(const param_type value) const
    {
        return base::Find(value);
    }
    /**
     * @brief Removes element equivalent to the given one.
     * @param value
     * @return Whether the element has been removed.
     */
    bool Remove(const param_type value)
    {
        return base::RemoveUnique(value) > 0;
    }
    /**
     * @brief Removes element at given position.
     * @param where
     * @return Iterator to removed element.
     */
    iterator Remove(const_iterator where)
    {
        return base::Remove(where);
    }
};

template<details::LessComparable T, typename Allocator = StandardAllocator<size_t>>
class MultiSet : private FlatTree<T, details::SetKeyOfValue<T>, std::less<T>, Allocator>
{
    using base                      = FlatTree<T, details::SetKeyOfValue<T>, std::less<T>, Allocator>;
    using underlying_container_type = typename base::container_type;
    using key_compare               = typename base::key_compare;

public:
    using value_type                = typename base::value_type;
    using size_type                 = typename base::size_type;
    using param_type                = typename base::param_type;
    using allocator_type            = typename base::allocator_type;
    using iterator                  = typename base::iterator;
    using const_iterator            = typename base::const_iterator;
    using reverse_iterator          = typename base::reverse_iterator;
    using const_reverse_iterator    = typename base::const_reverse_iterator;

    using base::begin;
    using base::end;
    using base::cbegin;
    using base::cend;
    using base::rbegin;
    using base::rend;
    using base::crbegin;
    using base::crend;

    using base::Size;
    using base::size;
    using base::Capacity;
    using base::Count;
    using base::Reserve;
    using base::Clear;
    using base::ShrinkToFit;

    /**
     * @brief Constructor, initialize allocator.
     * @param alloc
     */
    explicit MultiSet(const allocator_type& alloc = allocator_type()) : base(alloc) {};
    /**
     * @brief Constructor, initialize with given capacity.
     * @param capacity
     * @param alloc
     */
    explicit MultiSet(size_type capacity, const allocator_type& alloc = allocator_type()) : base(key_compare(), capacity, alloc) {};
    /**
     * @brief Constructor from an initializer
     * @param initializer
     * @param alloc
     */
    MultiSet(const std::initializer_list<value_type >& initializer, const allocator_type& alloc = allocator_type())
        : base(key_compare(), false, initializer, alloc) {}
    /**
     * @brief Constructor from a range
     * @tparam RangeType
     * @param range
     * @param alloc
     */
    template<std::ranges::bidirectional_range RangeType>
    explicit MultiSet(const RangeType& range, const allocator_type& alloc = allocator_type())
        : base(key_compare(), false, range, alloc) {}

    template<typename AnyAllocator>
    MultiSet& operator= (const MultiSet<value_type, AnyAllocator>& right)
    {
        base::operator=(right);
        return *this;
    }

    template<typename AnyAllocator>
    MultiSet& operator= (MultiSet<value_type, AnyAllocator>&& right)
    {
        base::operator=(std::forward<Set<value_type, AnyAllocator>>(right));
        return *this;
    }
    /**
     * @brief Inserts element to set.
     * @param value
     * @return Iterator to insert element.
     */
    iterator Insert(const param_type value)
    {
        return base::InsertEqual(value);
    }
    /**
     * @brief Inserts element to set.
     * @param value
     * @return Iterator to insert element.
     */
    iterator Insert(value_type&& value)
    {
        return base::InsertEqual(std::forward<value_type>(value));
    }
    /**
     * @brief Inserts each element from the range to set.
     * @tparam RangeType
     * @param range
     */
    template<std::ranges::forward_range RangeType>
    void Insert(const RangeType& range)
    {
        base::InsertEqual(range);
    }
    /**
     * @brief Returns whether set contains equivalently element.
     * @param value
     * @return
     */
    NODISCARD bool Contains(const param_type value) const
    {
        return base::Find(value) != end();
    }
    /**
     * @brief Finds first element equivalent to the given one.
     * @param value
     * @return Iterator to found element. Iterator to the end otherwise.
     */
    NODISCARD iterator Find(const param_type value)
    {
        return base::Find(value);
    }
    /**
     * @brief Finds first element equivalent to the given one.
     * @param value
     * @return Iterator to found element. Iterator to the end otherwise.
     */
    NODISCARD const_iterator Find(const param_type value) const
    {
        return base::Find(value);
    }
    /**
     * @brief Removes element equivalent to the given one.
     * @param value
     * @return Number of removed element.
     */
    size_type Remove(const param_type value)
    {
        return base::Remove(value);
    }
    /**
     * @brief Removes element at given position.
     * @param value
     * @return Iterator to removed element.
     */
    iterator Remove(const_iterator value)
    {
        return base::Remove(value);
    }
};

}
