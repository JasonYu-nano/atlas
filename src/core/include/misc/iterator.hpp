// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <iterator>

#include "core_def.hpp"

namespace atlas
{

template<typename Iter>
class WrapIterator
{
public:
    typedef Iter                                                            iterator_type;
    typedef typename std::iterator_traits<iterator_type>::value_type        value_type;
    typedef typename std::iterator_traits<iterator_type>::difference_type   difference_type;
    typedef typename std::iterator_traits<iterator_type>::pointer           pointer;
    typedef typename std::iterator_traits<iterator_type>::reference         reference;
    typedef typename std::iterator_traits<iterator_type>::iterator_category iterator_category;
    typedef std::contiguous_iterator_tag                                    iterator_concept;

    explicit WrapIterator(const Iter& it) : it_(it) {}

    reference       operator*   () const { return *it_; }

    pointer         operator->  () const { return std::to_address(it_); }

    WrapIterator&   operator++  () { ++it_; return *this; }

    WrapIterator    operator++  (int32) const { WrapIterator temp = *this; ++(*this); return temp; }

    WrapIterator&   operator--  () { --it_; return *this; }
    WrapIterator    operator--  (int32) const { WrapIterator temp = *this; --(*this); return temp; }

    WrapIterator    operator+   (difference_type n) const { WrapIterator temp = *this; temp.it_ += n; return temp; }
    WrapIterator&   operator+=  (difference_type n) { it_ += n; return *this; }

    WrapIterator    operator-   (difference_type n) const { WrapIterator temp = *this; temp, it_ -= n; return temp; }
    WrapIterator&   operator-=  (difference_type n) { it_ -= n; return *this; }

    reference       operator[]  (difference_type n) { return it_[n]; }

    bool            operator==  (const WrapIterator& right) const { return it_ == right.it_; }
    bool            operator!=  (const WrapIterator& right) const { return it_ != right.it_; }
    bool            operator<   (const WrapIterator& right) const { return it_ < right.it_; }
    bool            operator>   (const WrapIterator& right) const { return it_ > right.it_; }
    bool            operator<=  (const WrapIterator& right) const { return !(it_ > right.it_); }
    bool            operator>=  (const WrapIterator& right) const { return !(it_ < right.it_); }
    difference_type operator-   (const WrapIterator& right) const { return it_ - right.it_; }

private:
    iterator_type it_;
};

}