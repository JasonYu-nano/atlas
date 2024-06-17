// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "boost/unordered/detail/foa/table.hpp"
#include "boost/unordered/detail/foa/flat_map_types.hpp"

#include "memory/allocator.hpp"

namespace atlas
{

template <typename Key, typename Value, typename Allocator = HeapAllocator<void>>
class UnorderedMap
{
    using map_types = boost::unordered::detail::foa::flat_map_types<Key, Value>;
    using table_type = boost::unordered::detail::foa::table<map_types, std::hash<Key>, std::equal_to<Key>,
        typename AllocatorRebind<Allocator, typename map_types::value_type>::type>;
public:
    using key_type          = typename map_types::key_type;
    using value_type        = Value;
    using init_type         = typename map_types::init_type;
    using size_type         = typename table_type::size_type;
    using difference_type   = typename table_type::difference_type;
    using hasher            = typename table_type::hasher;
    using key_equal         = typename table_type::key_equal;
    using allocator_type    = typename table_type::allocator_type;
    using reference         = typename table_type::reference;
    using const_reference   = typename table_type::const_reference;
    using pointer           = typename table_type::pointer;
    using const_pointer     = typename table_type::const_pointer;
    using iterator          = typename table_type::iterator;
    using const_iterator    = typename table_type::const_iterator;
private:
    using key_param_type    = typename CallTraits<key_type>::param_type;
public:
    /**
     * @brief Constructor, initialize allocator.
     * @param alloc
     */
    explicit UnorderedMap(const allocator_type& alloc = allocator_type()) : UnorderedMap(0, alloc) {}
    /**
     * @brief Constructor, initialize hash bucket size and allocator.
     * @param bucket_size
     * @param alloc
     */
    explicit UnorderedMap(size_type bucket_size, const allocator_type& alloc = allocator_type())
        : table_(bucket_size, hasher(), key_equal(), alloc) {}
    /**
     * @brief Constructor from an initializer
     * @param initializer
     * @param bucket_size
     * @param alloc
     */
    explicit UnorderedMap(const std::initializer_list<init_type>& initializer, size_type bucket_size = 0, const allocator_type& alloc = allocator_type())
        : UnorderedMap(bucket_size, alloc)
    {
        insert(initializer);
    }
    /**
     * @brief Constructor from a range
     * @tparam RangeType
     * @param range
     * @param n
     * @param alloc
     */
    template <std::ranges::input_range RangeType>
    explicit UnorderedMap(RangeType range, size_type n = 0, const allocator_type& alloc = allocator_type())
        : UnorderedMap(n, alloc)
    {
        insert(range);
    }

    UnorderedMap(const UnorderedMap& right) : table_(right.table_) {};
    UnorderedMap(UnorderedMap&& right) noexcept : table_(std::move(right.table_)) {};
    ~UnorderedMap() = default;

    UnorderedMap& operator= (const UnorderedMap& right)
    {
        table_ = right.table_;
        return *this;
    }

    UnorderedMap& operator= (UnorderedMap&& right) noexcept
    {
        table_ = std::move(right.table_);
        return *this;
    }
    /**
     * @brief Get number of elements in map.
     * @return
     */
    NODISCARD size_type size() const
    {
        return table_.size();
    }
    /**
     * @brief Get maximum number of elements in map.
     * @return
     */
    NODISCARD constexpr size_type max_size() const { return table_.max_size(); }
    /**
     * @brief Get maximum number of elements in map.
     * @return
     */
    NODISCARD size_type capacity() const
    {
        return table_.capacity();
    }
    /**
     * @brief Clear all elements.
     */
    void clear()
    {
        table_.clear();
    }
    /**
     * @brief Reserves memory such that the map can contain at least number elements.
     * @param new_capacity
     */
    void reserve(size_type new_capacity)
    {
        table_.reserve(new_capacity);
    }
    /**
     * @brief Inserts value only if there is no key equivalently.
     * @param key
     * @param value
     * @param already_in_set Optional pointer to bool that will be set depending on whether element is already in map.
     * @return
     */
    iterator insert(const key_type& key, const value_type& value, bool* already_in_set = nullptr)
    {
        auto ret = table_.insert({key, value});
        if (already_in_set)
        {
            *already_in_set = !ret.second;
        }
        return ret.first;
    }
    /**
     * @brief Inserts value only if there is no key equivalently.
     * @param key
     * @param value
     * @param already_in_set Optional pointer to bool that will be set depending on whether element is already in map.
     * @return
     */
    iterator insert(const key_type& key, value_type&& value, bool* already_in_set = nullptr)
    {
        auto ret = table_.insert({key, std::forward<value_type>(value)});
        if (already_in_set)
        {
            *already_in_set = !ret.second;
        }
        return ret.first;
    }
    /**
     * @brief Inserts value only if there is no key equivalently.
     * @param key
     * @param value
     * @param already_in_set Optional pointer to bool that will be set depending on whether element is already in map.
     * @return
     */
    iterator insert(key_type&& key, const value_type& value, bool* already_in_set = nullptr)
    {
        auto ret = table_.insert({std::forward<key_type>(key), value});
        if (already_in_set)
        {
            *already_in_set = !ret.second;
        }
        return ret.first;
    }
    /**
     * @brief Inserts value only if there is no key equivalently.
     * @param key
     * @param value
     * @param already_in_set Optional pointer to bool that will be set depending on whether element is already in map.
     * @return
     */
    iterator insert(key_type&& key, value_type&& value, bool* already_in_set = nullptr)
    {
        auto ret = table_.insert({std::forward<key_type>(key), std::forward<value_type>(value)});
        if (already_in_set)
        {
            *already_in_set = !ret.second;
        }
        return ret.first;
    }
    /**
     * @brief Inserts each element from the range to map.
     * @tparam RangeType
     * @param range
     */
    template<std::ranges::input_range RangeType>
    void insert(const RangeType& range)
    {
        table_.insert(std::ranges::begin(range), std::ranges::end(range));
    }
    /**
     * @brief Finds the element associated with given key, or if none exists, adds a value using the default constructor.
     * @param key
     * @return
     */
    iterator find_or_insert(const key_type& key)
    {
        iterator it = table_.find(key);
        if (it == end())
        {
            it = table_.insert({key, value_type()});
        }
        return it;
    }
    /**
     * @brief Finds the element associated with given key, or if none exists, adds a value using the default constructor.
     * @param key
     * @return
     */
    iterator find_or_insert(key_type&& key)
    {
        iterator it = table_.find(key);
        if (it == end())
        {
            it = table_.insert({std::forward<key_type>(key), value_type()});
        }
        return it;
    }
    /**
     * @brief Returns whether map contains equivalently element.
     * @param key
     * @return
     */
    NODISCARD bool contains(const key_param_type key) const
    {
        return table_.find(key) != end();
    }
    /**
     * @brief Finds the element associated with given key.
     * @param key
     * @return
     */
    NODISCARD iterator find(const key_param_type key)
    {
        return table_.find(key);
    }
    /**
     * @brief Finds the element associated with given key.
     * @param key
     * @return
     */
    NODISCARD const_iterator find(const key_param_type key) const
    {
        return table_.find(key);
    }
    /**
     * @brief Finds the value associated with given key.
     * @param key
     * @return
     */
    NODISCARD value_type* find_value(const key_param_type key)
    {
        iterator it = table_.find(key);
        return it == end() ? nullptr : &it->second;
    }
    /**
     * @brief Finds the value associated with given key.
     * @param key
     * @return
     */
    NODISCARD const value_type* find_value(const key_param_type key) const
    {
        const_iterator it = table_.find(key);
        return it == end() ? nullptr : &it->second;
    }
    /**
     * @brief Finds the value associated with given key.
     * @param key
     * @return
     */
    NODISCARD value_type& find_value_ref(const key_param_type key)
    {
        iterator it = table_.find(key);
        ASSERT(it == end());
        return it->second;
    }
    /**
     * @brief Finds the value associated with given key.
     * @param key
     * @return
     */
    NODISCARD const value_type& find_value_ref(const key_param_type key) const
    {
        iterator it = table_.find(key);
        ASSERT(it == end());
        return it->second;
    }
    /**
     * @brief Removes element associated with given key.
     * @param value
     * @return
     */
    bool remove(const typename CallTraits<key_type>::param_type value)
    {
        return table_.erase(value);
    }
    /**
     * @brief Removes element at given position.
     * @param where
     * @return
     */
    iterator remove(const_iterator where)
    {
        return table_.erase(where);
    }

    NODISCARD iterator begin() { return table_.begin(); }
    NODISCARD const_iterator begin() const{ return table_.cbegin(); }
    NODISCARD iterator end() { return table_.end(); }
    NODISCARD const_iterator end() const { return table_.cend(); }
    NODISCARD const_iterator cbegin() const { return table_.cbegin(); }
    NODISCARD const_iterator cend() const { return table_.cend(); }
private:
    table_type table_;
};

} // namespace atlas
