// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <algorithm>

namespace atlas::algorithm
{


/**
 * @brief Moves the elements from the sorted range [first1, last1) which are not found in the sorted
 * range [first2, last2) to the range beginning at first1.
 * The resulting range is also sorted. Equivalent elements from range 1 are moved past to end
 * of the result,
 * that is, if some element is found m times in [first1, last1) and n times in [first2, last2),
 * it will be moved to result exactly max(m-n, 0) times.
 * The resulting range cannot overlap with either of the input ranges.
 */
template<typename Iter, typename Compare>
Iter set_difference(Iter first1, Iter last1, Iter first2, Iter last2, Iter result, const Compare& cmp)
{
    while (first1 != last1)
    {
        if (first2 == last2)
        {
            //unique_copy-like sequence with forward iterators but don't write i
            //to result before comparing as moving *i could alter the value in i.
            Iter i = first1;
            while (++first1 != last1)
            {
                if (cmp(*i, *first1))
                {
                    *result = *i;
                    ++result;
                    i = first1;
                }
            }
            *result = *i;
            ++result;
            break;
        }

        if (cmp(*first1, *first2))
        {
            //Skip equivalent elements in range1 but don't write i
            //to result before comparing as moving *i could alter the value in i.
            Iter i = first1;
            while (++first1 != last1)
            {
                if (cmp(*i, *first1))
                {
                    break;
                }
            }
            *result = *i;
            ++result;
        }
        else
        {
            if (cmp(*first2, *first1))
            {
                ++first2;
            }
            else
            {
                ++first1;
            }
        }
    }
    return result;
}

template<typename Iter, typename Compare>
Iter inplace_set_difference(Iter first1, Iter last1, Iter first2, Iter last2, const Compare& cmp)
{
    while (first1 != last1)
    {
        //Skip copying from range 1 if no element has to be skipped
        if (first2 == last2)
        {
            //unique-like algorithm for the remaining range 1
            Iter result = first1;
            while (++first1 != last1)
            {
                if (cmp(*result, *first1) && ++result != first1)
                {
                    *result = std::move(*first1);
                }
            }
            return ++result;
        }
        else if (cmp(*first2, *first1))
        {
            ++first2;
        }
        else if (cmp(*first1, *first2))
        {
            //skip any adjacent equivalent element in range 1
            Iter result = first1;
            if (++first1 != last1 && !cmp(*result, *first1)) {
                //Some elements from range 1 must be skipped, no longer an inplace operation
                while (++first1 != last1 && !cmp(*result, *first1)){}
                return set_difference(first1, last1, first2, last2, ++result, cmp);
            }
        }
        else
        {
            Iter result = first1;
            //Some elements from range 1 must be skipped, no longer an inplace operation
            while (++first1 != last1 && !cmp(*result, *first1)){}
            //An element from range 1 must be skipped, no longer an inplace operation
            return set_difference(first1, last1, first2, last2, result, cmp);
        }
    }
    return first1;
}

}
