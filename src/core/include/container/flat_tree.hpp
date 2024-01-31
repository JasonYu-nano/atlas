// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "memory/allocator.hpp"

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

template<typename KeyOfValue, typename Compare, typename UnderlyingContainer>
class FlatTree
{
public:
    using container_type            = UnderlyingContainer;
    using value_type                = typename container_type::value_type;
    using pointer                   = typename container_type::pointer;
    using const_pointer             = typename container_type::const_pointer;
    using reference                 = typename container_type::reference;
    using const_reference           = typename container_type::const_reference;
    using param_type                = typename container_type::param_type;
    using key_type                  = typename KeyOfValue::type;
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
    explicit FlatTree() : pair_() {};
    explicit FlatTree(const key_compare& compare) : pair_(compare) {};
    explicit FlatTree(const allocator_type& alloc) : pair_(container_type(alloc)) {};
    FlatTree(const key_compare& compare, const allocator_type& alloc) : pair_(compare, container_type(alloc)) {};
    FlatTree(const FlatTree& right) : pair_(right.pair_) {};
    FlatTree(FlatTree&& right) noexcept : pair_(std::move(right.pair_)) {};


    iterator InsertUnique(const param_type value);
    iterator InsertUnique(value_type&& value);

private:
    bool FindInsertPosition(const key_type& key, const_iterator& position)
    {
        return FindInsertPosition();
    }

    bool FindInsertPosition(const_iterator begin, const_iterator end, const key_type& key, const_iterator& position)
    {
        const key_compare& key_cmp = this->priv_key_comp();
        position = FindLowerBound(begin, end, key);
        return position == end || key_cmp(key, KeyOfValue()(*position));
    }

    template<typename Iter>
    Iter FindLowerBound(Iter first, const Iter last, const key_type& key) const
    {
        const Compare& key_cmp = this->m_data.get_comp();
        KeyOfValue key_extract;
        size_type len = static_cast<size_type>(last - first);
        Iter middle;

        while (len)
        {
            size_type step = len >> 1;
            middle = first;
            middle += difference_type(step);

            if (key_cmp(key_extract(*middle), key))
            {
                first = ++middle;
                len -= step + 1;
            }
            else
            {
                len = step;
            }
        }
        return first;
    }

    CompressionPair<value_compare, container_type> pair_;
};

}
