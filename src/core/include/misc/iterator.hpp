// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <iterator>

#include "core_def.hpp"

namespace atlas
{

template<typename Iter>
class PointerIterator
{
public:
    typedef Iter                                                            iterator_type;
    typedef typename std::iterator_traits<iterator_type>::value_type        value_type;
    typedef typename std::iterator_traits<iterator_type>::difference_type   difference_type;
    typedef typename std::iterator_traits<iterator_type>::pointer           pointer;
    typedef typename std::iterator_traits<iterator_type>::reference         reference;
    typedef const reference                                                 const_reference;
    typedef typename std::iterator_traits<iterator_type>::iterator_category iterator_category;
    typedef std::contiguous_iterator_tag                                    iterator_concept;

    PointerIterator() : it_(Iter()) {}
    explicit PointerIterator(const Iter& it) : it_(it) {}
    PointerIterator(const PointerIterator& right) = default;

    reference       operator*   () const { return *it_; }

    pointer         operator->  () const { return std::to_address(it_); }

    PointerIterator&   operator++  () { ++it_; return *this; }

    PointerIterator    operator++  (int32) const { PointerIterator temp = *this; ++(*this); return temp; }

    PointerIterator&   operator--  () { --it_; return *this; }
    PointerIterator    operator--  (int32) const { PointerIterator temp = *this; --(*this); return temp; }

    PointerIterator    operator+   (difference_type n) const { PointerIterator temp = *this; temp.it_ += n; return temp; }
    friend PointerIterator operator+(difference_type n, PointerIterator it) noexcept { it += n; return it; }
    PointerIterator&   operator+=  (difference_type n) { it_ += n; return *this; }

    PointerIterator    operator-   (difference_type n) const { PointerIterator temp = *this; temp.it_ -= n; return temp; }
    friend PointerIterator operator-(difference_type n, PointerIterator it) noexcept { it -= n; return it; }
    PointerIterator&   operator-=  (difference_type n) { it_ -= n; return *this; }

    reference       operator[]  (difference_type n) { return it_[n]; }
    const_reference operator[]  (difference_type n) const { return it_[n]; }

    bool            operator==  (const PointerIterator& right) const { return it_ == right.it_; }
    bool            operator!=  (const PointerIterator& right) const { return it_ != right.it_; }
    bool            operator<   (const PointerIterator& right) const { return it_ < right.it_; }
    bool            operator>   (const PointerIterator& right) const { return it_ > right.it_; }
    bool            operator<=  (const PointerIterator& right) const { return !(it_ > right.it_); }
    bool            operator>=  (const PointerIterator& right) const { return !(it_ < right.it_); }
    difference_type operator-   (const PointerIterator& right) const { return it_ - right.it_; }

private:
    iterator_type it_;
};

}