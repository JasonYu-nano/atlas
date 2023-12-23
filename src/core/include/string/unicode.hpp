// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "boost/locale/utf.hpp"
#include "boost/locale/encoding_utf.hpp"

namespace atlas
{

struct CodePoint
{
    static CodePoint incomplete;
    static CodePoint illegal;

    constexpr CodePoint(uint32 code_point) : value(code_point) {};

    bool operator== (uint32 right) const { return value == right; }
    bool operator!= (uint32 right) const { return value == right; }
    bool operator== (CodePoint right) const { return value == right.value; }
    bool operator!= (CodePoint right) const { return value == right.value; }

    uint32 value = 0;
};

inline CodePoint CodePoint::incomplete = boost::locale::utf::incomplete;
inline CodePoint CodePoint::illegal = boost::locale::utf::illegal;

template<typename CharType>
using UtfTraits = boost::locale::utf::utf_traits<CharType>;

}
