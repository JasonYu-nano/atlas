// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <iterator>

#include "core_def.hpp"

namespace atlas
{

template<typename T>
class ConstPointerIterator
{
public:
    typedef std::remove_cvref<T>::type                              value_type;
    typedef ptrdiff_t                                               difference_type;
    typedef const value_type*                                       pointer;
    typedef const value_type&                                       reference;
    typedef std::contiguous_iterator_tag                            iterator_category;
    typedef std::contiguous_iterator_tag                            iterator_concept;

    ConstPointerIterator() : it_(nullptr) {}
    explicit ConstPointerIterator(pointer it) : it_(it) {}
    ConstPointerIterator(const ConstPointerIterator& right) = default;

    reference               operator*   () const { return *it_; }
    pointer                 operator->  () const { return std::to_address(it_); }
    ConstPointerIterator&   operator++  () { ++it_; return *this; }
    ConstPointerIterator    operator++  (int32) { ConstPointerIterator temp = *this; ++(*this); return temp; }
    ConstPointerIterator&   operator--  () { --it_; return *this; }
    ConstPointerIterator    operator--  (int32) { ConstPointerIterator temp = *this; --(*this); return temp; }
    ConstPointerIterator    operator+   (difference_type n) const { ConstPointerIterator temp = *this; temp.it_ += n; return temp; }
    friend ConstPointerIterator operator+(difference_type n, ConstPointerIterator it) noexcept { it += n; return it; }
    ConstPointerIterator&   operator+=  (difference_type n) { it_ += n; return *this; }
    ConstPointerIterator    operator-   (difference_type n) const { ConstPointerIterator temp = *this; temp.it_ -= n; return temp; }
    friend ConstPointerIterator operator-(difference_type n, ConstPointerIterator it) noexcept { it -= n; return it; }
    ConstPointerIterator&   operator-=  (difference_type n) { it_ -= n; return *this; }

    reference       operator[]  (difference_type n) const { return it_[n]; }

    bool            operator==  (const ConstPointerIterator& right) const { return it_ == right.it_; }
    bool            operator!=  (const ConstPointerIterator& right) const { return it_ != right.it_; }
    bool            operator<   (const ConstPointerIterator& right) const { return it_ < right.it_; }
    bool            operator>   (const ConstPointerIterator& right) const { return it_ > right.it_; }
    bool            operator<=  (const ConstPointerIterator& right) const { return it_ <= right.it_; }
    bool            operator>=  (const ConstPointerIterator& right) const { return it_ >= right.it_; }
    difference_type operator-   (const ConstPointerIterator& right) const { return it_ - right.it_; }

    pointer GetPointer() { return std::to_address(it_); }

protected:
    pointer it_;
};

template<typename T>
class PointerIterator : public ConstPointerIterator<T>
{
    using base = ConstPointerIterator<T>;
public:
    typedef typename base::value_type          value_type;
    typedef typename base::difference_type     difference_type;
    typedef value_type*                        pointer;
    typedef value_type&                        reference;
    typedef typename base::iterator_category   iterator_category;
    typedef typename base::iterator_concept    iterator_concept;

    PointerIterator() : base() {}
    explicit PointerIterator(pointer it) : base(it) {}
    PointerIterator(const PointerIterator& right) = default;

    // NOLINTBEGIN()
    reference           operator*   () const { return const_cast<reference>(base::operator*()); }
    pointer             operator->  () const { return const_cast<pointer>(base::operator->()); }
    PointerIterator&    operator++  () { base::operator++(); return *this; }
    PointerIterator     operator++  (int32) { PointerIterator temp = *this; base::operator++(); return temp; }
    PointerIterator&    operator--  () { base::operator--(); return *this; }
    PointerIterator     operator--  (int32) { PointerIterator temp = *this; base::operator--(); return temp; }
    PointerIterator     operator+   (difference_type n) const { PointerIterator temp = *this; temp += n; return temp; }
    friend PointerIterator operator+(difference_type n, PointerIterator it) noexcept { it += n; return it; }
    PointerIterator&    operator+=  (difference_type n) { base::operator+=(n); return *this; }
    PointerIterator     operator-   (difference_type n) const { PointerIterator temp = *this; temp -= n; return temp; }
    friend PointerIterator operator-(difference_type n, PointerIterator it) noexcept { it -= n; return it; }
    PointerIterator&    operator-=  (difference_type n) { base::operator-=(n); return *this; }
    reference           operator[]  (difference_type n) const { return const_cast<reference>(base::operator[]); }
    // NOLINTEND()
    bool            operator==  (const PointerIterator& right) const { return base::operator==(right); }
    bool            operator!=  (const PointerIterator& right) const { return base::operator!=(right); }
    bool            operator<   (const PointerIterator& right) const { return base::operator<(right); }
    bool            operator>   (const PointerIterator& right) const { return base::operator>(right); }
    bool            operator<=  (const PointerIterator& right) const { return base::operator<=(right); }
    bool            operator>=  (const PointerIterator& right) const { return base::operator>=(right); }
    difference_type operator-   (const PointerIterator& right) const { return base::operator-(right); }
};

template<typename Iter>
Iter* IteratorToPointer(Iter* it)
{
    return it;
}

template<std::input_iterator Iter>
std::iterator_traits<Iter>::pointer IteratorToPointer(Iter it)
{
    return it.operator->();
}

} // namespace atlas