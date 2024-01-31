// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "container/flat_tree.hpp"
#include "container/array.hpp"

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
    using reference = std::add_const_t<std::add_lvalue_reference_t<T>>;

    reference operator()(reference t) const
    {
        return t;
    }
};
}

template<details::LessComparable T, typename Allocator>
class Set : public FlatTree<details::SetKeyOfValue<T>, std::less<T>, Array<T, Allocator>>
{
    using base = FlatTree<details::SetKeyOfValue<T>, std::less<T>, Array<T, Allocator>>;
    using underlying_container_type = base::container_type;

public:
    explicit Set(const Allocator& alloc = Allocator()) : base(alloc) {};
};

}
