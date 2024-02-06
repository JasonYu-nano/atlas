// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "container/flat_tree.hpp"
#include "utility/concepts.hpp"

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

template<LessComparable Key, typename Value, typename Allocator = StandardAllocator<size_t>>
class Map : private FlatTree<std::pair<Key, Value>, details::KeyOfPair<Key>, std::less<Key>, Allocator>
{
    using base                      = FlatTree<std::pair<Key, Value>, details::KeyOfPair<Key>, std::less<Key>, Allocator>;
    using underlying_container_type = typename base::container_type;
    using key_compare               = typename base::key_compare;
    using key_param_type            = typename base::key_param_type;

public:
    using key_type                  = typename base::key_type;
    using value_type                = Value;
    using pair_type                 = typename base::value_type;
    using size_type                 = typename base::size_type;
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
    explicit Map(const allocator_type& alloc = allocator_type()) : base(alloc) {};
    /**
     * @brief Constructor, initialize with given capacity.
     * @param capacity
     * @param alloc
     */
    explicit Map(size_type capacity, const allocator_type& alloc = allocator_type())
        : base(key_compare(), capacity, alloc) {};
    /**
     * @brief Constructor from an initializer
     * @param initializer
     * @param alloc
     */
    Map(const std::initializer_list<pair_type>& initializer, const allocator_type& alloc = allocator_type())
        : base(key_compare(), true, initializer, alloc) {}
    /**
     * @brief Constructor from a range
     * @tparam RangeType
     * @param range
     * @param alloc
     */
    template<std::ranges::bidirectional_range RangeType>
    explicit Map(const RangeType& range, const allocator_type& alloc = allocator_type())
        : base(key_compare(), true, range, alloc) {}

    template<typename AnyAllocator>
    explicit Map(const Map<key_type, value_type, AnyAllocator>& right) : base(right) {}

    template<typename AnyAllocator>
    explicit Map(Map<key_type, value_type, AnyAllocator>&& right) noexcept
        : base(std::forward<Map<key_type, value_type, AnyAllocator>>(right)) {}


    template<typename AnyAllocator>
    Map& operator= (const Map<key_type, value_type, AnyAllocator>& right)
    {
        base::operator=(right);
        return *this;
    }

    template<typename AnyAllocator>
    Map& operator= (Map<key_type, value_type, AnyAllocator>&& right)
    {
        base::operator=(std::forward<Map<key_type, value_type, AnyAllocator>>(right));
        return *this;
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
        return base::InsertUnique(pair_type(key, value), already_in_map);
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
        return base::InsertUnique(pair_type(std::forward<key_type>(key), value), already_in_map);
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
        return base::InsertUnique(pair_type(key, std::forward<value_type>(value)), already_in_map);
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
        return base::InsertUnique(pair_type(std::forward<key_type>(key), std::forward<value_type>(value)), already_in_map);
    }
    /**
     * @brief Inserts each element from the range to map.
     * @tparam RangeType
     * @param range
     */
    template<std::ranges::forward_range RangeType>
    void Insert(const RangeType& range)
    {
        base::InsertUnique(range);
    }
    /**
     * @brief Finds the element associated with given key, or if none exists, adds a value using the default constructor.
     * @param key
     * @return Iterator to found element.
     */
    iterator FindOrInsert(const key_type& key)
    {
        iterator it = base::Find(key);
        if (it == base::end())
        {
            it = base::InsertUnique(pair_type(key, value_type()));
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
        iterator it = base::Find(key);
        if (it == base::end())
        {
            it = base::InsertUnique(pair_type(std::forward<key_type>(key), value_type()));
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
        return base::Find(key) != end();
    }
    /**
     * @brief Finds the element associated with given key.
     * @param key
     * @return Iterator to found element. Iterator to the end otherwise.
     */
    NODISCARD iterator Find(const key_param_type key)
    {
        return base::Find(key);
    }
    /**
     * @brief Finds the element associated with given key.
     * @param key
     * @return Iterator to found element. Iterator to the end otherwise.
     */
    NODISCARD const_iterator Find(const key_param_type key) const
    {
        return base::Find(key);
    }
    /**
     * @brief Finds the value associated with given key.
     * @param key
     * @return Pointer to the found value. Returns null otherwise.
     */
    NODISCARD value_type* FindValue(const key_param_type key)
    {
        iterator it = base::Find(key);
        return it == base::end() ? nullptr : &it->second;
    }
    /**
     * @brief Finds the value associated with given key.
     * @param key
     * @return Pointer to the found value. Returns null otherwise.
     */
    NODISCARD const value_type* FindValue(const key_param_type key) const
    {
        iterator it = base::Find(key);
        return it == base::end() ? nullptr : &it->second;
    }
    /**
     * @brief Finds the value associated with given key.
     * @param key
     * @return Reference to the found value.
     */
    NODISCARD value_type& FindValueRef(const key_param_type key)
    {
        iterator it = base::Find(key);
        ASSERT(it != base::end());
        return it->second;
    }
    /**
     * @brief Finds the value associated with given key.
     * @param key
     * @return Reference to the found value.
     */
    NODISCARD const value_type& FindValueRef(const key_param_type key) const
    {
        iterator it = base::Find(key);
        ASSERT(it != base::end());
        return it->second;
    }
    /**
     * @brief Removes element associated with given key.
     * @param key
     * @return Whether the element has been removed.
     */
    bool Remove(const key_param_type key)
    {
        return base::RemoveUnique(key) > 0;
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

template<LessComparable Key, typename Value, typename Allocator = StandardAllocator<size_t>>
class MultiMap : private FlatTree<std::pair<Key, Value>, details::KeyOfPair<Key>, std::less<Key>, Allocator>
{
    using base                      = FlatTree<std::pair<Key, Value>, details::KeyOfPair<Key>, std::less<Key>, Allocator>;
    using underlying_container_type = typename base::container_type;
    using key_compare               = typename base::key_compare;
    using key_param_type            = typename base::key_param_type;

public:
    using key_type                  = typename base::key_type;
    using value_type                = Value;
    using pair_type                 = typename base::value_type;
    using size_type                 = typename base::size_type;
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
    explicit MultiMap(const allocator_type& alloc = allocator_type()) : base(alloc) {};
    /**
     * @brief Constructor, initialize with given capacity.
     * @param capacity
     * @param alloc
     */
    explicit MultiMap(size_type capacity, const allocator_type& alloc = allocator_type())
        : base(key_compare(), capacity, alloc) {};
    /**
     * @brief Constructor from an initializer
     * @param initializer
     * @param alloc
     */
    MultiMap(const std::initializer_list<pair_type>& initializer, const allocator_type& alloc = allocator_type())
        : base(key_compare(), false, initializer, alloc) {}
    /**
     * @brief Constructor from a range
     * @tparam RangeType
     * @param range
     * @param alloc
     */
    template<std::ranges::bidirectional_range RangeType>
    explicit MultiMap(const RangeType& range, const allocator_type& alloc = allocator_type())
        : base(key_compare(), false, range, alloc) {}

    template<typename AnyAllocator>
    explicit MultiMap(const MultiMap<key_type, value_type, AnyAllocator>& right) : base(right) {}

    template<typename AnyAllocator>
    explicit MultiMap(MultiMap<key_type, value_type, AnyAllocator>&& right) noexcept
        : base(std::forward<MultiMap<key_type, value_type, AnyAllocator>>(right)) {}


    template<typename AnyAllocator>
    MultiMap& operator= (const MultiMap<key_type, value_type, AnyAllocator>& right)
    {
        base::operator=(right);
        return *this;
    }

    template<typename AnyAllocator>
    MultiMap& operator= (MultiMap<key_type, value_type, AnyAllocator>&& right)
    {
        base::operator=(std::forward<MultiMap<key_type, value_type, AnyAllocator>>(right));
        return *this;
    }

    /**
     * @brief Inserts value.
     * @param key
     * @param value
     * @return Iterator to insert element.
     */
    iterator Insert(const key_type& key, const value_type& value)
    {
        return base::InsertEqual(pair_type(key, value));
    }
    /**
     * @brief Inserts value.
     * @param key
     * @param value
     * @return Iterator to insert element.
     */
    iterator Insert(key_type&& key, const value_type& value)
    {
        return base::InsertEqual(pair_type(std::forward<key_type>(key), value));
    }
    /**
     * @brief Inserts value.
     * @param key
     * @param value
     * @return Iterator to insert element.
     */
    iterator Insert(const key_type& key, value_type&& value)
    {
        return base::InsertEqual(pair_type(key, std::forward<value_type>(value)));
    }
    /**
     * @brief Inserts value.
     * @param key
     * @param value
     * @return Iterator to insert element.
     */
    iterator Insert(key_type&& key, value_type&& value)
    {
        return base::InsertEqual(pair_type(std::forward<key_type>(key), std::forward<value_type>(value)));
    }
    /**
     * @brief Inserts each element from the range to map.
     * @tparam RangeType
     * @param range
     */
    template<std::ranges::forward_range RangeType>
    void Insert(const RangeType& range)
    {
        base::InsertEqual(range);
    }
    /**
     * @brief Finds the element associated with given key, or if none exists, adds a value using the default constructor.
     * @param key
     * @return Iterator to found element.
     */
    iterator FindOrInsert(const key_type& key)
    {
        iterator it = base::Find(key);
        if (it == base::end())
        {
            it = base::InsertEqual(pair_type(key, value_type()));
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
        iterator it = base::Find(key);
        if (it == base::end())
        {
            it = base::InsertEqual(pair_type(std::forward<key_type>(key), value_type()));
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
        return base::Find(key) != end();
    }
    /**
     * @brief Finds first element associated with given key.
     * @param key
     * @return Iterator to found element. Iterator to the end otherwise.
     */
    NODISCARD iterator Find(const key_param_type key)
    {
        return base::Find(key);
    }
    /**
     * @brief Finds first element associated with given key.
     * @param key
     * @return Iterator to found element. Iterator to the end otherwise.
     */
    NODISCARD const_iterator Find(const key_param_type key) const
    {
        return base::Find(key);
    }
    /**
     * @brief Finds first value associated with given key.
     * @param key
     * @return Pointer to the found value. Returns null otherwise.
     */
    NODISCARD value_type* FindValue(const key_param_type key)
    {
        iterator it = base::Find(key);
        return it == base::end() ? nullptr : &it->second;
    }
    /**
     * @brief Finds first value associated with given key.
     * @param key
     * @return Pointer to the found value. Returns null otherwise.
     */
    NODISCARD const value_type* FindValue(const key_param_type key) const
    {
        iterator it = base::Find(key);
        return it == base::end() ? nullptr : &it->second;
    }
    /**
     * @brief Finds first value associated with given key.
     * @param key
     * @return Reference to the found value.
     */
    NODISCARD value_type& FindValueRef(const key_param_type key)
    {
        iterator it = base::Find(key);
        ASSERT(it != base::end());
        return it->second;
    }
    /**
     * @brief Finds first value associated with given key.
     * @param key
     * @return Reference to the found value.
     */
    NODISCARD const value_type& FindValueRef(const key_param_type key) const
    {
        iterator it = base::Find(key);
        ASSERT(it != base::end());
        return it->second;
    }
    /**
     * @brief Removes element associated with given key.
     * @param key
     * @return Whether the element has been removed.
     */
    size_type Remove(const key_param_type key)
    {
        return base::Remove(key);
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
}
