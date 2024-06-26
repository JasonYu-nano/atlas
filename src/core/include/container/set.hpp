// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "container/flat_tree.hpp"

namespace atlas
{

namespace details
{

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

template<typename Key, typename Allocator = HeapAllocator<void>, typename Compare = std::less<Key>>
class Set
{
    using tree_type                 = details::FlatTree<Key, details::SetKeyOfValue<Key>, Compare, Allocator>;
    using key_compare               = typename tree_type::key_compare;

public:
    using value_type                = typename tree_type::value_type;
    using size_type                 = typename tree_type::size_type;
    using param_type                = typename tree_type::param_type;
    using allocator_type            = typename tree_type::allocator_type;
    using iterator                  = typename tree_type::iterator;
    using const_iterator            = typename tree_type::const_iterator;
    using reverse_iterator          = typename tree_type::reverse_iterator;
    using const_reverse_iterator    = typename tree_type::const_reverse_iterator;

    /**
     * @brief Constructor, initialize allocator.
     * @param alloc
     */
    explicit Set(const allocator_type& alloc = allocator_type()) : tree_(alloc) {};
    /**
     * @brief Constructor, initialize with given capacity.
     * @param capacity
     * @param alloc
     */
    explicit Set(size_type capacity, const allocator_type& alloc = allocator_type())
        : tree_(key_compare(), capacity, alloc) {};
    /**
     * @brief Constructor from an initializer
     * @param initializer
     * @param alloc
     */
    Set(const std::initializer_list<value_type>& initializer, const allocator_type& alloc = allocator_type())
        : tree_(key_compare(), true, initializer, alloc) {}
    /**
     * @brief Constructor from a range
     * @tparam RangeType
     * @param range
     * @param alloc
     */
    template<std::ranges::forward_range RangeType>
    explicit Set(const RangeType& range, const allocator_type& alloc = allocator_type())
        : tree_(key_compare(), true, range, alloc) {}

    template<typename AnyAllocator>
    explicit Set(const Set<value_type, AnyAllocator>& right) : tree_(right) {}

    template<typename AnyAllocator>
    explicit Set(Set<value_type, AnyAllocator>&& right) noexcept
        : tree_(std::forward<Set<value_type, AnyAllocator>>(right)) {}

    template<typename AnyAllocator>
    Set& operator= (const Set<value_type, AnyAllocator>& right)
    {
        tree_ = right.tree_;
        return *this;
    }

    template<typename AnyAllocator>
    Set& operator= (Set<value_type, AnyAllocator>&& right)
    {
        tree_ = std::move(right.tree_);
        return *this;
    }
    /**
     * @brief Get number of elements in set.
     * @return
     */
    NODISCARD size_type size() const
    {
        return tree_.size();
    }
    /**
     * @brief Get maximum number of elements in set.
     * @return
     */
    NODISCARD size_type max_size() const
    {
        return tree_.max_size();
    }
    /**
     * @brief Get capacity of set.
     * @return
     */
    NODISCARD size_type capacity() const
    {
        return tree_.capacity();
    }
    /**
     * @brief Reserves memory such that the set can contain at least number elements.
     * @param new_capacity
     */
    void reserve(size_type new_capacity)
    {
        tree_.reserve(new_capacity);
    }
    /**
     * @brief Clear all elements.
     * @param reset_capacity Deallocate the remain capacity. Default is false.
     */
    void clear(bool reset_capacity = false)
    {
        tree_.clear(reset_capacity);
    }
    /**
     * @brief Shrinks the container's used memory to smallest possible to store elements currently in it.
     */
    void shrink_to_fit()
    {
        tree_.shrink_to_fit();
    }
    /**
     * @brief Inserts element only if there is no element equivalently.
     * @param value
     * @param already_in_set Optional pointer to bool that will be set depending on whether element is already in set.
     * @return Iterator to insert element.
     */
    iterator insert(const param_type value, bool* already_in_set = nullptr)
    {
        return tree_.insert_unique(value, already_in_set);
    }
    /**
     * @brief Inserts element only if there is no element equivalently.
     * @param value
     * @param already_in_set Optional pointer to bool that will be set depending on whether element is already in set.
     * @return Iterator to insert element.
     */
    iterator insert(value_type&& value, bool* already_in_set = nullptr)
    {
        return tree_.insert_unique(std::forward<value_type>(value), already_in_set);
    }
    /**
     * @brief Inserts each element from the range only if there is no element equivalently.
     * @tparam RangeType
     * @param range
     */
    template<std::ranges::forward_range RangeType>
    void insert(const RangeType& range)
    {
        tree_.insert_unique(range);
    }
    /**
     * @brief Returns whether set contains equivalently element.
     * @param value
     * @return
     */
    NODISCARD bool contains(const param_type value) const
    {
        return tree_.find(value) != end();
    }
    /**
     * @brief Finds element equivalent to the given one.
     * @param value
     * @return Iterator to found element. Iterator to the end otherwise.
     */
    NODISCARD iterator find(const param_type value)
    {
        return tree_.find(value);
    }
    /**
     * @brief Finds element equivalent to the given one.
     * @param value
     * @return Iterator to found element. Iterator to the end otherwise.
     */
    NODISCARD const_iterator find(const param_type value) const
    {
        return tree_.find(value);
    }
    /**
     * @brief Removes element equivalent to the given one.
     * @param value
     * @return Whether the element has been removed.
     */
    bool remove(const param_type value)
    {
        return tree_.remove_unique(value) > 0;
    }
    /**
     * @brief Removes element at given position.
     * @param where
     * @return Iterator to removed element.
     */
    iterator remove(const_iterator where)
    {
        return tree_.remove(where);
    }

    NODISCARD iterator begin() { return tree_.begin(); }
    NODISCARD const_iterator begin() const{ return tree_.cbegin(); }
    NODISCARD iterator end() { return tree_.end(); }
    NODISCARD const_iterator end() const { return tree_.cend(); }
    NODISCARD const_iterator cbegin() const { return tree_.cbegin(); }
    NODISCARD const_iterator cend() const { return tree_.cend(); }
    NODISCARD reverse_iterator rbegin() const { return tree_.rbegin(); }
    NODISCARD reverse_iterator rend() const { return tree_.rend(); }
    NODISCARD const_reverse_iterator crbegin() const { return tree_.crbegin(); }
    NODISCARD const_reverse_iterator crend() const { return tree_.crend(); }

private:
    tree_type tree_;
};

template<typename Key, typename Allocator = HeapAllocator<void>, typename Compare = std::less<Key>>
class MultiSet
{
    using tree_type                 = details::FlatTree<Key, details::SetKeyOfValue<Key>, Compare, Allocator>;
    using key_compare               = typename tree_type::key_compare;

public:
    using value_type                = typename tree_type::value_type;
    using size_type                 = typename tree_type::size_type;
    using param_type                = typename tree_type::param_type;
    using allocator_type            = typename tree_type::allocator_type;
    using iterator                  = typename tree_type::iterator;
    using const_iterator            = typename tree_type::const_iterator;
    using reverse_iterator          = typename tree_type::reverse_iterator;
    using const_reverse_iterator    = typename tree_type::const_reverse_iterator;

    /**
     * @brief Constructor, initialize allocator.
     * @param alloc
     */
    explicit MultiSet(const allocator_type& alloc = allocator_type()) : tree_(alloc) {};
    /**
     * @brief Constructor, initialize with given capacity.
     * @param capacity
     * @param alloc
     */
    explicit MultiSet(size_type capacity, const allocator_type& alloc = allocator_type()) : tree_(key_compare(), capacity, alloc) {};
    /**
     * @brief Constructor from an initializer
     * @param initializer
     * @param alloc
     */
    MultiSet(const std::initializer_list<value_type >& initializer, const allocator_type& alloc = allocator_type())
        : tree_(key_compare(), false, initializer, alloc) {}
    /**
     * @brief Constructor from a range
     * @tparam RangeType
     * @param range
     * @param alloc
     */
    template<std::ranges::bidirectional_range RangeType>
    explicit MultiSet(const RangeType& range, const allocator_type& alloc = allocator_type())
        : tree_(key_compare(), false, range, alloc) {}

    template<typename AnyAllocator>
    explicit MultiSet(const MultiSet<value_type, AnyAllocator>& right) : tree_(right) {}

    template<typename AnyAllocator>
    explicit MultiSet(MultiSet<value_type, AnyAllocator>&& right) noexcept
        : tree_(std::forward<MultiSet<value_type, AnyAllocator>>(right)) {}

    template<typename AnyAllocator>
    MultiSet& operator= (const MultiSet<value_type, AnyAllocator>& right)
    {
        tree_ = right.tree_;
        return *this;
    }

    template<typename AnyAllocator>
    MultiSet& operator= (MultiSet<value_type, AnyAllocator>&& right)
    {
        tree_ = std::move(right.tree_);
        return *this;
    }
    /**
     * @brief Get number of elements in set.
     * @return
     */
    NODISCARD size_type size() const
    {
        return tree_.size();
    }
    /**
     * @brief Get maximum number of elements in set.
     * @return
     */
    NODISCARD size_type max_size() const
    {
        return tree_.max_size();
    }
    /**
     * @brief Get capacity of set.
     * @return
     */
    NODISCARD size_type capacity() const
    {
        return tree_.capacity();
    }
    /**
     * @brief Reserves memory such that the set can contain at least number elements.
     * @param new_capacity
     */
    void reserve(size_type new_capacity)
    {
        tree_.reserve(new_capacity);
    }
    /**
     * @brief Clear all elements.
     * @param reset_capacity Deallocate the remain capacity. Default is false.
     */
    void clear(bool reset_capacity = false)
    {
        tree_.clear(reset_capacity);
    }
    /**
     * @brief Shrinks the container's used memory to smallest possible to store elements currently in it.
     */
    void shrink_to_fit()
    {
        tree_.shrink_to_fit();
    }
    /**
     * @brief Inserts element to set.
     * @param value
     * @return Iterator to insert element.
     */
    iterator insert(const param_type value)
    {
        return tree_.insert_equal(value);
    }
    /**
     * @brief Inserts element to set.
     * @param value
     * @return Iterator to insert element.
     */
    iterator insert(value_type&& value)
    {
        return tree_.insert_equal(std::forward<value_type>(value));
    }
    /**
     * @brief Inserts each element from the range to set.
     * @tparam RangeType
     * @param range
     */
    template<std::ranges::forward_range RangeType>
    void insert(const RangeType& range)
    {
        tree_.insert_equal(range);
    }
    /**
     * @brief Returns whether set contains equivalently element.
     * @param value
     * @return
     */
    NODISCARD bool contains(const param_type value) const
    {
        return tree_.find(value) != end();
    }
    /**
     * @brief Get number of elements equivalent to the given one.
     * @param value
     * @return
     */
    size_type count(const param_type value) const
    {
        return tree_.count(value);
    }
    /**
     * @brief Finds first element equivalent to the given one.
     * @param value
     * @return Iterator to found element. Iterator to the end otherwise.
     */
    NODISCARD iterator find(const param_type value)
    {
        return tree_.find(value);
    }
    /**
     * @brief Finds first element equivalent to the given one.
     * @param value
     * @return Iterator to found element. Iterator to the end otherwise.
     */
    NODISCARD const_iterator find(const param_type value) const
    {
        return tree_.find(value);
    }
    /**
     * @brief Removes element equivalent to the given one.
     * @param value
     * @return Number of removed element.
     */
    size_type remove(const param_type value)
    {
        return tree_.remove(value);
    }
    /**
     * @brief Removes element at given position.
     * @param value
     * @return Iterator to removed element.
     */
    iterator remove(const_iterator value)
    {
        return tree_.remove(value);
    }

    NODISCARD iterator begin() { return tree_.begin(); }
    NODISCARD const_iterator begin() const{ return tree_.cbegin(); }
    NODISCARD iterator end() { return tree_.end(); }
    NODISCARD const_iterator end() const { return tree_.cend(); }
    NODISCARD const_iterator cbegin() const { return tree_.cbegin(); }
    NODISCARD const_iterator cend() const { return tree_.cend(); }
    NODISCARD reverse_iterator rbegin() const { return tree_.rbegin(); }
    NODISCARD reverse_iterator rend() const { return tree_.rend(); }
    NODISCARD const_reverse_iterator crbegin() const { return tree_.crbegin(); }
    NODISCARD const_reverse_iterator crend() const { return tree_.crend(); }
private:
    tree_type tree_;
};

} // namespace atlas
