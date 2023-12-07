// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

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

template<typename T1, typename T2, int>
class CompressionPairImpl
{
public:
    using FirstType             = T1;
    using SecondType            = T2;
    using FirstReference        = CallTraits<FirstType>::Reference;
    using SecondReference       = CallTraits<SecondType>::Reference;
    using FirstConstReference   = CallTraits<FirstType>::ConstReference;
    using SecondConstReference  = CallTraits<SecondType>::ConstReference;
    using FirstParamType        = CallTraits<FirstType>::ParamType;
    using SecondParamType       = CallTraits<SecondType>::ParamType;

    CompressionPairImpl() = default;
    explicit CompressionPairImpl(FirstParamType first) : first_(first) {}
    explicit CompressionPairImpl(SecondParamType second) : second_(second) {}
    CompressionPairImpl(FirstParamType first, SecondParamType second) : first_(first), second_(second) {}

    FirstReference First() { return first_; }
    FirstConstReference First() const { return first_; }
    SecondReference Second() { return second_; }
    SecondConstReference Second() const { return second_; }

private:
    FirstType first_;
    SecondType second_;
};

template<typename T1, typename T2>
class CompressionPairImpl<T1, T2, 1> : protected std::remove_cv_t<T1>
{
public:
    using FirstType             = T1;
    using SecondType            = T2;
    using FirstReference        = CallTraits<FirstType>::Reference;
    using SecondReference       = CallTraits<SecondType>::Reference;
    using FirstConstReference   = CallTraits<FirstType>::ConstReference;
    using SecondConstReference  = CallTraits<SecondType>::ConstReference;
    using FirstParamType        = CallTraits<FirstType>::ParamType;
    using SecondParamType       = CallTraits<SecondType>::ParamType;

    CompressionPairImpl() = default;
    explicit CompressionPairImpl(FirstParamType first) : FirstType(first) {}
    explicit CompressionPairImpl(SecondParamType second) : second_(second) {}
    CompressionPairImpl(FirstParamType first, SecondParamType second) : FirstType(first), second_(second) {}

    FirstReference First() { return *this; }
    FirstConstReference First() const { return *this; }
    SecondReference Second() { return second_; }
    SecondConstReference Second() const { return second_; }

private:
    SecondType second_;
};

template<typename T1, typename T2>
class CompressionPairImpl<T1, T2, 2> : protected std::remove_cv_t<T2>
{
public:
    using FirstType             = T1;
    using SecondType            = T2;
    using FirstReference        = CallTraits<FirstType>::Reference;
    using SecondReference       = CallTraits<SecondType>::Reference;
    using FirstConstReference   = CallTraits<FirstType>::ConstReference;
    using SecondConstReference  = CallTraits<SecondType>::ConstReference;
    using FirstParamType        = CallTraits<FirstType>::ParamType;
    using SecondParamType       = CallTraits<SecondType>::ParamType;

    CompressionPairImpl() = default;
    explicit CompressionPairImpl(FirstParamType first) : first_(first) {}
    explicit CompressionPairImpl(SecondParamType second) : SecondType(second) {}
    CompressionPairImpl(FirstParamType first, SecondParamType second) : first_(first), SecondType(second) {}

    FirstReference First() { return first_; }
    FirstConstReference First() const { return first_; }
    SecondReference Second() { return *this; }
    SecondConstReference Second() const { return *this; }

private:
    FirstType first_;
};

template<typename T1, typename T2>
class CompressionPairImpl<T1, T2, 3> : protected std::remove_cv_t<T1>, protected std::remove_cv_t<T2>
{
public:
    using FirstType             = T1;
    using SecondType            = T2;
    using FirstReference        = CallTraits<FirstType>::Reference;
    using SecondReference       = CallTraits<SecondType>::Reference;
    using FirstConstReference   = CallTraits<FirstType>::ConstReference;
    using SecondConstReference  = CallTraits<SecondType>::ConstReference;
    using FirstParamType        = CallTraits<FirstType>::ParamType;
    using SecondParamType       = CallTraits<SecondType>::ParamType;

    CompressionPairImpl() = default;
    explicit CompressionPairImpl(FirstParamType first) : FirstType(first) {}
    explicit CompressionPairImpl(SecondParamType second) : SecondType(second) {}
    CompressionPairImpl(FirstParamType first, SecondParamType second) : FirstType(first), SecondType(second) {}

    FirstReference First() { return *this; }
    FirstConstReference First() const { return *this; }
    SecondReference Second() { return *this; }
    SecondConstReference Second() const { return *this; }
};

}

template<typename T1, typename T2, bool = std::is_empty_v<T1> && !std::is_final_v<T1>>
class CompressionPair : private details::CompressionPairImpl<T1, T2, details::CompressionPairSwitch<T1, T2>::value>
{
    using Super = details::CompressionPairImpl<T1, T2, details::CompressionPairSwitch<T1, T2>::value>;
public:
    using FirstType             = Super::FirstType;
    using SecondType            = Super::SecondType;
    using FirstReference        = Super::FirstReference;
    using SecondReference       = Super::SecondReference;
    using FirstConstReference   = Super::FirstConstReference;
    using SecondConstReference  = Super::SecondConstReference;
    using FirstParamType        = Super::FirstParamType;
    using SecondParamType       = Super::SecondParamType;

    CompressionPair() : Super() {}
    explicit CompressionPair(FirstParamType first) : Super(first) {}
    explicit CompressionPair(SecondParamType second) : Super(second) {}
    CompressionPair(FirstParamType first, SecondParamType second) : Super(first, second) {}

    FirstReference First() { Super::First(); }
    FirstConstReference First() const { Super::First(); }

    SecondReference Second() { Super::Second(); }
    SecondConstReference Second() const { Super::Second(); }
};

}