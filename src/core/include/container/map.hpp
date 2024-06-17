// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "container/flat_tree.hpp"

namespace atlas
{

namespace details
{

template<typename T>
struct KeyOfPair
{
    using type = T;

    template<typename PairType>
    const type& operator()(const PairType& t) const
    {
        return t.first;
    }
};

}

template<typename Key, typename Value, typename Allocator = HeapAllocator<void>, typename Compare = std::less<Key>>
class Map
{
    using tree_type                 = details::FlatTree<std::pair<Key, Value>, details::KeyOfPair<Key>, Compare, Allocator>;
    using key_compare               = typename tree_type::key_compare;
    using key_param_type            = typename tree_type::key_param_type;

public:
    using key_type                  = typename tree_type::key_type;
    using value_type                = Value;
    using pair_type                 = typename tree_type::value_type;
    using size_type                 = typename tree_type::size_type;
    using allocator_type            = typename tree_type::allocator_type;
    using iterator                  = typename tree_type::iterator;
    using const_iterator            = typename tree_type::const_iterator;
    using reverse_iterator          = typename tree_type::reverse_iterator;
    using const_reverse_iterator    = typename tree_type::const_reverse_iterator;

    /**
     * @brief Constructor, initialize allocator.
     * @param alloc
     */
    explicit Map(const allocator_type& alloc = allocator_type()) : tree_(alloc) {};
    /**
     * @brief Constructor, initialize with given capacity.
     * @param capacity
     * @param alloc
     */
    explicit Map(size_type capacity, const allocator_type& alloc = allocator_type())
        : tree_(key_compare(), capacity, alloc) {};
    /**
     * @brief Constructor from an initializer
     * @param initializer
     * @param alloc
     */
    Map(const std::initializer_list<pair_type>& initializer, const allocator_type& alloc = allocator_type())
        : tree_(key_compare(), true, initializer, alloc) {}
    /**
     * @brief Constructor from a range
     * @tparam RangeType
     * @param range
     * @param alloc
     */
    template<std::ranges::forward_range RangeType>
    explicit Map(const RangeType& range, const allocator_type& alloc = allocator_type())
        : tree_(key_compare(), true, range, alloc) {}

    template<typename AnyAllocator>
    explicit Map(const Map<key_type, value_type, AnyAllocator>& right) : tree_(right) {}

    template<typename AnyAllocator>
    explicit Map(Map<key_type, value_type, AnyAllocator>&& right) noexcept
        : tree_(std::forward<Map<key_type, value_type, AnyAllocator>>(right)) {}


    template<typename AnyAllocator>
    Map& operator= (const Map<key_type, value_type, AnyAllocator>& right)
    {
        tree_ = right.tree_;
        return *this;
    }

    template<typename AnyAllocator>
    Map& operator= (Map<key_type, value_type, AnyAllocator>&& right)
    {
        tree_ = std::move(right.tree_);
        return *this;
    }
    /**
     * @brief Get number of elements in map.
     * @return
     */
    NODISCARD size_type Size() const
    {
        return tree_.size();
    }
    /**
     * @brief Get number of elements in map.
     * @return
     */
    NODISCARD DO_NOT_USE_DIRECTLY size_type size() const
    {
        return tree_.size();
    }
    /**
     * @brief Get maximum number of elements in map.
     * @return
     */
    NODISCARD size_type MaxSize() const
    {
        return tree_.max_size();
    }
    /**
     * @brief Get maximum number of elements in map.
     * @return
     */
    NODISCARD DO_NOT_USE_DIRECTLY size_type max_size() const
    {
        return tree_.max_size();
    }
    /**
     * @brief Get capacity of map.
     * @return
     */
    NODISCARD size_type Capacity() const
    {
        return tree_.capacity();
    }
    /**
     * @brief Reserves memory such that the map can contain at least number elements.
     * @param new_capacity
     */
    void Reserve(size_type new_capacity)
    {
        tree_.reserve(new_capacity);
    }
    /**
     * @brief Clear all elements.
     * @param reset_capacity Deallocate the remain capacity. Default is false.
     */
    void Clear(bool reset_capacity = false)
    {
        tree_.clear(reset_capacity);
    }
    /**
     * @brief Shrinks the container's used memory to smallest possible to store elements currently in it.
     */
    void ShrinkToFit()
    {
        tree_.shrink_to_fit();
    }
    /**
     * @brief Inserts value only if there is no key equivalently.
     * @param key
     * @param value
     * @param already_in_map Optional pointer to bool that will be set depending on whether element is already in map.
     * @return Iterator to insert element.
     */
    iterator Insert(const key_type& key, const value_type& value, bool* already_in_map = nullptr)
    {
        return tree_.InsertUnique(pair_type(key, value), already_in_map);
    }
    /**
     * @brief Inserts value only if there is no key equivalently.
     * @param key
     * @param value
     * @param already_in_map Optional pointer to bool that will be set depending on whether element is already in map.
     * @return Iterator to insert element.
     */
    iterator Insert(key_type&& key, const value_type& value, bool* already_in_map = nullptr)
    {
        return tree_.InsertUnique(pair_type(std::forward<key_type>(key), value), already_in_map);
    }
    /**
     * @brief Inserts value only if there is no key equivalently.
     * @param key
     * @param value
     * @param already_in_map Optional pointer to bool that will be set depending on whether element is already in map.
     * @return Iterator to insert element.
     */
    iterator Insert(const key_type& key, value_type&& value, bool* already_in_map = nullptr)
    {
        return tree_.InsertUnique(pair_type(key, std::forward<value_type>(value)), already_in_map);
    }
    /**
     * @brief Inserts value only if there is no key equivalently.
     * @param key
     * @param value
     * @param already_in_map Optional pointer to bool that will be set depending on whether element is already in map.
     * @return Iterator to insert element.
     */
    iterator Insert(key_type&& key, value_type&& value, bool* already_in_map = nullptr)
    {
        return tree_.InsertUnique(pair_type(std::forward<key_type>(key), std::forward<value_type>(value)), already_in_map);
    }
    /**
     * @brief Inserts each element from the range to map.
     * @tparam RangeType
     * @param range
     */
    template<std::ranges::forward_range RangeType>
    void Insert(const RangeType& range)
    {
        tree_.InsertUnique(range);
    }
    /**
     * @brief Finds the element associated with given key, or if none exists, adds a value using the default constructor.
     * @param key
     * @return Iterator to found element.
     */
    iterator FindOrInsert(const key_type& key)
    {
        iterator it = tree_.find(key);
        if (it == tree_.end())
        {
            it = tree_.InsertUnique(pair_type(key, value_type()));
        }
        return it;
    }
    /**
     * @brief Finds the element associated with given key, or if none exists, adds a value using the default constructor.
     * @param key
     * @return Iterator to found element.
     */
    iterator FindOrInsert(key_type&& key)
    {
        iterator it = tree_.find(key);
        if (it == tree_.end())
        {
            it = tree_.InsertUnique(pair_type(std::forward<key_type>(key), value_type()));
        }
        return it;
    }
    /**
     * @brief Returns whether map contains equivalently element.
     * @param key
     * @return
     */
    NODISCARD bool Contains(const key_param_type key) const
    {
        return tree_.find(key) != end();
    }
    /**
     * @brief Finds the element associated with given key.
     * @param key
     * @return Iterator to found element. Iterator to the end otherwise.
     */
    NODISCARD iterator Find(const key_param_type key)
    {
        return tree_.find(key);
    }
    /**
     * @brief Finds the element associated with given key.
     * @param key
     * @return Iterator to found element. Iterator to the end otherwise.
     */
    NODISCARD const_iterator Find(const key_param_type key) const
    {
        return tree_.find(key);
    }
    /**
     * @brief Finds the value associated with given key.
     * @param key
     * @return Pointer to the found value. Returns null otherwise.
     */
    NODISCARD value_type* FindValue(const key_param_type key)
    {
        iterator it = tree_.find(key);
        return it == tree_.end() ? nullptr : &it->second;
    }
    /**
     * @brief Finds the value associated with given key.
     * @param key
     * @return Pointer to the found value. Returns null otherwise.
     */
    NODISCARD const value_type* FindValue(const key_param_type key) const
    {
        iterator it = tree_.find(key);
        return it == tree_.end() ? nullptr : &it->second;
    }
    /**
     * @brief Finds the value associated with given key.
     * @param key
     * @return Reference to the found value.
     */
    NODISCARD value_type& FindValueRef(const key_param_type key)
    {
        iterator it = tree_.find(key);
        ASSERT(it != tree_.end());
        return it->second;
    }
    /**
     * @brief Finds the value associated with given key.
     * @param key
     * @return Reference to the found value.
     */
    NODISCARD const value_type& FindValueRef(const key_param_type key) const
    {
        iterator it = tree_.find(key);
        ASSERT(it != tree_.end());
        return it->second;
    }
    /**
     * @brief Removes element associated with given key.
     * @param key
     * @return Whether the element has been removed.
     */
    bool Remove(const key_param_type key)
    {
        return tree_.RemoveUnique(key) > 0;
    }
    /**
     * @brief Removes element at given position.
     * @param where
     * @return Iterator to removed element.
     */
    iterator Remove(const_iterator where)
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

template<typename Key, typename Value, typename Allocator = HeapAllocator<void>, typename Compare = std::less<Key>>
class MultiMap
{
    using tree_type                 = details::FlatTree<std::pair<Key, Value>, details::KeyOfPair<Key>, Compare, Allocator>;
    using key_compare               = typename tree_type::key_compare;
    using key_param_type            = typename tree_type::key_param_type;

public:
    using key_type                  = typename tree_type::key_type;
    using value_type                = Value;
    using pair_type                 = typename tree_type::value_type;
    using size_type                 = typename tree_type::size_type;
    using allocator_type            = typename tree_type::allocator_type;
    using iterator                  = typename tree_type::iterator;
    using const_iterator            = typename tree_type::const_iterator;
    using reverse_iterator          = typename tree_type::reverse_iterator;
    using const_reverse_iterator    = typename tree_type::const_reverse_iterator;

    /**
     * @brief Constructor, initialize allocator.
     * @param alloc
     */
    explicit MultiMap(const allocator_type& alloc = allocator_type()) : tree_(alloc) {};
    /**
     * @brief Constructor, initialize with given capacity.
     * @param capacity
     * @param alloc
     */
    explicit MultiMap(size_type capacity, const allocator_type& alloc = allocator_type())
        : tree_(key_compare(), capacity, alloc) {};
    /**
     * @brief Constructor from an initializer
     * @param initializer
     * @param alloc
     */
    MultiMap(const std::initializer_list<pair_type>& initializer, const allocator_type& alloc = allocator_type())
        : tree_(key_compare(), false, initializer, alloc) {}
    /**
     * @brief Constructor from a range
     * @tparam RangeType
     * @param range
     * @param alloc
     */
    template<std::ranges::bidirectional_range RangeType>
    explicit MultiMap(const RangeType& range, const allocator_type& alloc = allocator_type())
        : tree_(key_compare(), false, range, alloc) {}

    template<typename AnyAllocator>
    explicit MultiMap(const MultiMap<key_type, value_type, AnyAllocator>& right) : tree_(right) {}

    template<typename AnyAllocator>
    explicit MultiMap(MultiMap<key_type, value_type, AnyAllocator>&& right) noexcept
        : tree_(std::forward<MultiMap<key_type, value_type, AnyAllocator>>(right)) {}


    template<typename AnyAllocator>
    MultiMap& operator= (const MultiMap<key_type, value_type, AnyAllocator>& right)
    {
        tree_ = right.tree_;
        return *this;
    }

    template<typename AnyAllocator>
    MultiMap& operator= (MultiMap<key_type, value_type, AnyAllocator>&& right)
    {
        tree_ = std::move(right.tree_);
        return *this;
    }
    /**
     * @brief Get number of elements in map.
     * @return
     */
    NODISCARD size_type Size() const
    {
        return tree_.size();
    }
    /**
     * @brief Get number of elements in map.
     * @return
     */
    NODISCARD DO_NOT_USE_DIRECTLY size_type size() const
    {
        return tree_.size();
    }
    /**
     * @brief Get maximum number of elements in map.
     * @return
     */
    NODISCARD size_type MaxSize() const
    {
        return tree_.max_size();
    }
    /**
     * @brief Get maximum number of elements in map.
     * @return
     */
    NODISCARD DO_NOT_USE_DIRECTLY size_type max_size() const
    {
        return tree_.max_size();
    }
    /**
     * @brief Get capacity of map.
     * @return
     */
    NODISCARD size_type Capacity() const
    {
        return tree_.capacity();
    }
    /**
     * @brief Reserves memory such that the map can contain at least number elements.
     * @param new_capacity
     */
    void Reserve(size_type new_capacity)
    {
        tree_.reserve(new_capacity);
    }
    /**
     * @brief Clear all elements.
     * @param reset_capacity Deallocate the remain capacity. Default is false.
     */
    void Clear(bool reset_capacity = false)
    {
        tree_.clear(reset_capacity);
    }
    /**
     * @brief Shrinks the container's used memory to smallest possible to store elements currently in it.
     */
    void ShrinkToFit()
    {
        tree_.shrink_to_fit();
    }
    /**
     * @brief Inserts value.
     * @param key
     * @param value
     * @return Iterator to insert element.
     */
    iterator Insert(const key_type& key, const value_type& value)
    {
        return tree_.InsertEqual(pair_type(key, value));
    }
    /**
     * @brief Inserts value.
     * @param key
     * @param value
     * @return Iterator to insert element.
     */
    iterator Insert(key_type&& key, const value_type& value)
    {
        return tree_.InsertEqual(pair_type(std::forward<key_type>(key), value));
    }
    /**
     * @brief Inserts value.
     * @param key
     * @param value
     * @return Iterator to insert element.
     */
    iterator Insert(const key_type& key, value_type&& value)
    {
        return tree_.InsertEqual(pair_type(key, std::forward<value_type>(value)));
    }
    /**
     * @brief Inserts value.
     * @param key
     * @param value
     * @return Iterator to insert element.
     */
    iterator Insert(key_type&& key, value_type&& value)
    {
        return tree_.InsertEqual(pair_type(std::forward<key_type>(key), std::forward<value_type>(value)));
    }
    /**
     * @brief Inserts each element from the range to map.
     * @tparam RangeType
     * @param range
     */
    template<std::ranges::forward_range RangeType>
    void Insert(const RangeType& range)
    {
        tree_.InsertEqual(range);
    }
    /**
     * @brief Finds the element associated with given key, or if none exists, adds a value using the default constructor.
     * @param key
     * @return Iterator to found element.
     */
    iterator FindOrInsert(const key_type& key)
    {
        iterator it = tree_.find(key);
        if (it == tree_.end())
        {
            it = tree_.InsertEqual(pair_type(key, value_type()));
        }
        return it;
    }
    /**
     * @brief Finds the element associated with given key, or if none exists, adds a value using the default constructor.
     * @param key
     * @return Iterator to found element.
     */
    iterator FindOrInsert(key_type&& key)
    {
        iterator it = tree_.find(key);
        if (it == tree_.end())
        {
            it = tree_.InsertEqual(pair_type(std::forward<key_type>(key), value_type()));
        }
        return it;
    }
    /**
     * @brief Returns whether map contains equivalently element.
     * @param key
     * @return
     */
    NODISCARD bool Contains(const key_param_type key) const
    {
        return tree_.find(key) != end();
    }
    /**
     * @brief Get number of elements equivalent to the given one.
     * @param value
     * @return
     */
    size_type Count(const key_param_type value) const
    {
        return tree_.Count(value);
    }
    /**
     * @brief Finds first element associated with given key.
     * @param key
     * @return Iterator to found element. Iterator to the end otherwise.
     */
    NODISCARD iterator Find(const key_param_type key)
    {
        return tree_.find(key);
    }
    /**
     * @brief Finds first element associated with given key.
     * @param key
     * @return Iterator to found element. Iterator to the end otherwise.
     */
    NODISCARD const_iterator Find(const key_param_type key) const
    {
        return tree_.find(key);
    }
    /**
     * @brief Finds first value associated with given key.
     * @param key
     * @return Pointer to the found value. Returns null otherwise.
     */
    NODISCARD value_type* FindValue(const key_param_type key)
    {
        iterator it = tree_.find(key);
        return it == tree_.end() ? nullptr : &it->second;
    }
    /**
     * @brief Finds first value associated with given key.
     * @param key
     * @return Pointer to the found value. Returns null otherwise.
     */
    NODISCARD const value_type* FindValue(const key_param_type key) const
    {
        iterator it = tree_.find(key);
        return it == tree_.end() ? nullptr : &it->second;
    }
    /**
     * @brief Finds first value associated with given key.
     * @param key
     * @return Reference to the found value.
     */
    NODISCARD value_type& FindValueRef(const key_param_type key)
    {
        iterator it = tree_.find(key);
        ASSERT(it != tree_.end());
        return it->second;
    }
    /**
     * @brief Finds first value associated with given key.
     * @param key
     * @return Reference to the found value.
     */
    NODISCARD const value_type& FindValueRef(const key_param_type key) const
    {
        iterator it = tree_.find(key);
        ASSERT(it != tree_.end());
        return it->second;
    }
    /**
     * @brief Removes element associated with given key.
     * @param key
     * @return Whether the element has been removed.
     */
    size_type Remove(const key_param_type key)
    {
        return tree_.remove(key);
    }
    /**
     * @brief Removes element at given position.
     * @param where
     * @return Iterator to removed element.
     */
    iterator Remove(const_iterator where)
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

} // namespace atlas
