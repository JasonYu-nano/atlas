// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "core_def.hpp"
#include "utility/call_traits.hpp"

namespace atlas
{
namespace details
{

template<typename T1, typename T2, bool IsSame = std::is_same_v<T1, T2>,
    bool FirstEmpty = std::is_empty_v<T1> && !std::is_final_v<T1>, bool SecondEmpty = std::is_empty_v<T2> && !std::is_final_v<T2>>
struct CompressionPairSwitch
{
    static constexpr int32 value = 0;
};

template<typename T1, typename T2>
struct CompressionPairSwitch<T1, T2, false, true, false>
{
    static constexpr int32 value = 1;
};

template<typename T1, typename T2>
struct CompressionPairSwitch<T1, T2, false, false, true>
{
    static constexpr int32 value = 2;
};

template<typename T1, typename T2>
struct CompressionPairSwitch<T1, T2, false, true, true>
{
    static constexpr int32 value = 3;
};

template<typename T1, typename T2>
struct CompressionPairSwitch<T1, T2, true, false, false>
{
    static constexpr int32 value = 4;
};

template<typename T1, typename T2>
struct CompressionPairSwitch<T1, T2, true, true, true>
{
    static constexpr int32 value = 5;
};

template<typename T1, typename T2, int>
class CompressionPairImpl
{
public:
    using first_type             = T1;
    using second_type            = T2;
    using first_reference        = CallTraits<first_type>::reference;
    using second_reference       = CallTraits<second_type>::reference;
    using first_const_reference  = CallTraits<first_type>::const_reference;
    using second_const_reference = CallTraits<second_type>::const_reference;
    using first_param_type       = CallTraits<first_type>::param_type;
    using second_param_type      = CallTraits<second_type>::param_type;

    CompressionPairImpl() = default;
    explicit CompressionPairImpl(first_param_type first) : first_(first) {}
    explicit CompressionPairImpl(second_param_type second) : second_(second) {}
    CompressionPairImpl(first_param_type first, second_param_type second) : first_(first), second_(second) {}

    first_reference First() { return first_; }
    first_const_reference First() const { return first_; }
    second_reference Second() { return second_; }
    second_const_reference Second() const { return second_; }

private:
    first_type first_;
    second_type second_;
};

template<typename T1, typename T2>
class CompressionPairImpl<T1, T2, 1> : protected std::remove_cv_t<T1>
{
public:
    using first_type             = T1;
    using second_type            = T2;
    using first_reference        = CallTraits<first_type>::reference;
    using second_reference       = CallTraits<second_type>::reference;
    using first_const_reference  = CallTraits<first_type>::const_reference;
    using second_const_reference = CallTraits<second_type>::const_reference;
    using first_param_type       = CallTraits<first_type>::param_type;
    using second_param_type      = CallTraits<second_type>::param_type;

    CompressionPairImpl() = default;
    explicit CompressionPairImpl(first_param_type first) : first_type(first) {}
    explicit CompressionPairImpl(second_param_type second) : second_(second) {}
    CompressionPairImpl(first_param_type first, second_param_type second) : first_type(first), second_(second) {}

    first_reference First() { return *this; }
    first_const_reference First() const { return *this; }
    second_reference Second() { return second_; }
    second_const_reference Second() const { return second_; }

private:
    second_type second_;
};

template<typename T1, typename T2>
class CompressionPairImpl<T1, T2, 2> : protected std::remove_cv_t<T2>
{
public:
    using first_type             = T1;
    using second_type            = T2;
    using first_reference        = CallTraits<first_type>::reference;
    using second_reference       = CallTraits<second_type>::reference;
    using first_const_reference  = CallTraits<first_type>::const_reference;
    using second_const_reference = CallTraits<second_type>::const_reference;
    using first_param_type       = CallTraits<first_type>::param_type;
    using second_param_type      = CallTraits<second_type>::param_type;

    CompressionPairImpl() = default;
    explicit CompressionPairImpl(first_param_type first) : first_(first) {}
    explicit CompressionPairImpl(second_param_type second) : second_type(second) {}
    CompressionPairImpl(first_param_type first, second_param_type second) : first_(first), second_type(second) {}

    first_reference First() { return first_; }
    first_const_reference First() const { return first_; }
    second_reference Second() { return *this; }
    second_const_reference Second() const { return *this; }

private:
    first_type first_;
};

template<typename T1, typename T2>
class CompressionPairImpl<T1, T2, 3> : protected std::remove_cv_t<T1>, protected std::remove_cv_t<T2>
{
public:
    using first_type             = T1;
    using second_type            = T2;
    using first_reference        = CallTraits<first_type>::reference;
    using second_reference       = CallTraits<second_type>::reference;
    using first_const_reference  = CallTraits<first_type>::const_reference;
    using second_const_reference = CallTraits<second_type>::const_reference;
    using first_param_type       = CallTraits<first_type>::param_type;
    using second_param_type      = CallTraits<second_type>::param_type;

    CompressionPairImpl() = default;
    explicit CompressionPairImpl(first_param_type first) : first_type(first) {}
    explicit CompressionPairImpl(second_param_type second) : second_type(second) {}
    CompressionPairImpl(first_param_type first, second_param_type second) : first_type(first), second_type(second) {}

    first_reference First() { return *this; }
    first_const_reference First() const { return *this; }
    second_reference Second() { return *this; }
    second_const_reference Second() const { return *this; }
};

template<typename T1, typename T2>
class CompressionPairImpl<T1, T2, 4>
{
public:
    using first_type             = T1;
    using second_type            = T2;
    using first_reference        = CallTraits<first_type>::reference;
    using second_reference       = CallTraits<second_type>::reference;
    using first_const_reference  = CallTraits<first_type>::const_reference;
    using second_const_reference = CallTraits<second_type>::const_reference;
    using first_param_type       = CallTraits<first_type>::param_type;
    using second_param_type      = CallTraits<second_type>::param_type;

    CompressionPairImpl() = default;
    explicit CompressionPairImpl(first_param_type first) : first_(first), second_(first) {}
    CompressionPairImpl(first_param_type first, second_param_type second) : first_(first), second_(second) {}

    first_reference First() { return first_; }
    first_const_reference First() const { return first_; }
    second_reference Second() { return second_; }
    second_const_reference Second() const { return second_; }

private:
    first_type first_;
    second_type second_;
};

template<typename T1, typename T2>
class CompressionPairImpl<T1, T2, 5> : protected std::remove_cv_t<T1>
{
public:
    using first_type             = T1;
    using second_type            = T2;
    using first_reference        = CallTraits<first_type>::reference;
    using second_reference       = CallTraits<second_type>::reference;
    using first_const_reference  = CallTraits<first_type>::const_reference;
    using second_const_reference = CallTraits<second_type>::const_reference;
    using first_param_type       = CallTraits<first_type>::param_type;
    using second_param_type      = CallTraits<second_type>::param_type;

    CompressionPairImpl() = default;
    explicit CompressionPairImpl(first_param_type first) : first_type(first), second_(first) {}
    CompressionPairImpl(first_param_type first, second_param_type second) : first_type(first), second_(second) {}

    first_reference First() { return *this; }
    first_const_reference First() const { return *this; }
    second_reference Second() { return second_; }
    second_const_reference Second() const { return second_; }

private:
    second_type second_;
};

}

template<typename T1, typename T2, bool = std::is_same_v<T1, T2>>
class CompressionPair : private details::CompressionPairImpl<T1, T2, details::CompressionPairSwitch<T1, T2>::value>
{
    using Super = details::CompressionPairImpl<T1, T2, details::CompressionPairSwitch<T1, T2>::value>;
public:
    using first_type             = Super::first_type;
    using second_type            = Super::second_type;
    using first_reference        = Super::first_reference;
    using second_reference       = Super::second_reference;
    using first_const_reference  = Super::first_const_reference;
    using second_const_reference = Super::second_const_reference;
    using first_param_type       = Super::first_param_type;
    using second_param_type      = Super::second_param_type;

    CompressionPair() : Super() {}
    explicit CompressionPair(first_param_type first) : Super(first) {}
    explicit CompressionPair(second_param_type second) : Super(second) {}
    CompressionPair(first_param_type first, second_param_type second) : Super(first, second) {}

    using Super::First;
    using Super::Second;
};

template<typename T1, typename T2>
class CompressionPair<T1, T2, true> : private details::CompressionPairImpl<T1, T2, details::CompressionPairSwitch<T1, T2>::value>
{
    using Super = details::CompressionPairImpl<T1, T2, details::CompressionPairSwitch<T1, T2>::value>;
public:
    using first_type             = Super::first_type;
    using second_type            = Super::second_type;
    using first_reference        = Super::first_reference;
    using second_reference       = Super::second_reference;
    using first_const_reference  = Super::first_const_reference;
    using second_const_reference = Super::second_const_reference;
    using first_param_type       = Super::first_param_type;
    using second_param_type      = Super::second_param_type;

    CompressionPair() : Super() {}
    explicit CompressionPair(first_param_type first) : Super(first) {}
    CompressionPair(first_param_type first, second_param_type second) : Super(first, second) {}

    using Super::First;
    using Super::Second;
};

}