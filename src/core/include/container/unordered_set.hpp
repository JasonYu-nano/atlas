// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "boost/unordered/detail/foa/table.hpp"
#include "boost/unordered/detail/foa/flat_set_types.hpp"

#include "memory/allocator.hpp"
#include "utility/concepts.hpp"

namespace atlas
{

template <typename Key, typename Allocator = HeapAllocator<Key>>
class UnorderedSet
{
    using set_types = boost::unordered::detail::foa::flat_set_types<Key>;
    using table_type = boost::unordered::detail::foa::table<set_types, std::hash<Key>, std::equal_to<Key>, Allocator>;

public:
    using key_type = Key;
    using value_type = typename set_types::value_type;
    using init_type = typename set_types::init_type;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using hasher = std::hash<Key>;
    using key_equal = std::equal_to<Key>;
    using allocator_type = Allocator;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = typename boost::allocator_pointer<allocator_type>::type;
    using const_pointer = typename boost::allocator_const_pointer<allocator_type>::type;
    using iterator = typename table_type::iterator;
    using const_iterator = typename table_type::const_iterator;

private:
    using param_type = typename CallTraits<value_type>::param_type;
public:
    /**
     * @brief Constructor, initialize allocator.
     * @param alloc
     */
    explicit UnorderedSet(const allocator_type& alloc = allocator_type()) : UnorderedSet(0, alloc) {}
    /**
     * @brief Constructor, initialize hash bucket size and allocator.
     * @param bucket_size
     * @param alloc
     */
    explicit UnorderedSet(size_type bucket_size, const allocator_type& alloc = allocator_type())
        : table_(bucket_size, hasher(), key_equal(), alloc) {}
    /**
     * @brief Constructor from an initializer
     * @param initializer
     * @param bucket_size
     * @param alloc
     */
    explicit UnorderedSet(const std::initializer_list<value_type>& initializer, size_type bucket_size = 0, const allocator_type& alloc = allocator_type())
        : UnorderedSet(bucket_size, alloc)
    {
        Insert(initializer);
    }
    /**
     * @brief Constructor from a range
     * @tparam RangeType
     * @param range
     * @param n
     * @param alloc
     */
    template <std::ranges::input_range RangeType>
    explicit UnorderedSet(RangeType range, size_type n = 0, const allocator_type& alloc = allocator_type())
        : UnorderedSet(n, alloc)
    {
        Insert(range);
    }

    UnorderedSet(const UnorderedSet& right) : table_(right.table_) {};
    UnorderedSet(UnorderedSet&& right) noexcept : table_(std::move(right.table_)) {};
    ~UnorderedSet() = default;

    UnorderedSet& operator= (const UnorderedSet& right)
    {
        table_ = right.table_;
        return *this;
    }

    UnorderedSet& operator= (UnorderedSet&& right) noexcept
    {
        table_ = std::move(right.table_);
        return *this;
    }
    /**
     * @brief Inserts element only if there is no element equivalently.
     * @param value
     * @param already_in_set Optional pointer to bool that will be set depending on whether element is already in set.
     * @return
     */
    iterator Insert(const value_type& value, bool* already_in_set = nullptr)
    {
        auto ret = table_.insert(value);
        if (already_in_set)
        {
            *already_in_set = ret.second;
        }
        return ret.first;
    }
    /**
     * @brief Inserts element only if there is no element equivalently.
     * @param value
     * @param already_in_set Optional pointer to bool that will be set depending on whether element is already in set.
     * @return
     */
    iterator Insert(value_type&& value, bool* already_in_set = nullptr)
    {
        auto ret = table_.insert(std::forward<value_type>(value));
        if (already_in_set)
        {
            *already_in_set = ret.second;
        }
        return ret.first;
    }
    /**
     * @brief Inserts each element from the range only if there is no element equivalently.
     * @tparam RangeType
     * @param range
     */
    template<std::ranges::input_range RangeType>
    void Insert(const RangeType& range)
    {
        table_.insert(std::ranges::begin(range), std::ranges::end(range));
    }
    /**
     * @brief Returns whether set contains equivalently element.
     * @param value
     * @return
     */
    NODISCARD bool Contains(const param_type value) const
    {
        return table_.find(value) != end();
    }
    /**
     * @brief Finds element equivalent to the given one.
     * @param value
     * @return
     */
    NODISCARD iterator Find(const param_type value)
    {
        return table_.find(value);
    }
    /**
     * @brief Finds element equivalent to the given one.
     * @param value
     * @return
     */
    NODISCARD const_iterator Find(const param_type value) const
    {
        return table_.find(value);
    }
    /**
     * @brief Removes element equivalent to the given one.
     * @param value
     * @return
     */
    bool Remove(const param_type value)
    {
        return table_.erase(value);
    }
    /**
     * @brief Removes element at given position.
     * @param where
     * @return
     */
    iterator Remove(const_iterator where)
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
