// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "core_def.hpp"
#include "utility/call_traits.hpp"

namespace atlas
{
struct ZeroThenVariadicArgs {};
struct OneThenVariadicArgs {};

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

    template<typename... Arg>
    explicit CompressionPairImpl(Arg&&... arg) : first_{std::forward<Arg>(arg)...} {}

    template<typename... Arg>
    explicit CompressionPairImpl(ZeroThenVariadicArgs, Arg&&... arg) : second_{std::forward<Arg>(arg)...} {}

    template<typename Arg1, typename... Arg2>
    CompressionPairImpl(OneThenVariadicArgs, Arg1&& arg1, Arg2&&... arg2)
        : first_(std::forward<Arg1>(arg1)), second_{std::forward<Arg2>(arg2)...} {}

    first_reference first() { return first_; }
    first_const_reference first() const { return first_; }
    second_reference second() { return second_; }
    second_const_reference second() const { return second_; }

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

    template<typename... Arg>
    explicit CompressionPairImpl(Arg&&... arg) : first_type{std::forward<Arg>(arg)...} {}

    template<typename... Arg>
    explicit CompressionPairImpl(ZeroThenVariadicArgs, Arg&&... arg) : second_{std::forward<Arg>(arg)...} {}

    template<typename Arg1, typename... Arg2>
    CompressionPairImpl(OneThenVariadicArgs, Arg1&& arg1, Arg2&&... arg2)
        : first_type(std::forward<Arg1>(arg1)), second_{std::forward<Arg2>(arg2)...} {}

    first_reference first() { return *this; }
    first_const_reference first() const { return *this; }
    second_reference second() { return second_; }
    second_const_reference second() const { return second_; }

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

    template<typename... Arg>
    explicit CompressionPairImpl(Arg&&... arg) : first_{std::forward<Arg>(arg)...} {}

    template<typename... Arg>
    explicit CompressionPairImpl(ZeroThenVariadicArgs, Arg&&... arg) : second_type{std::forward<Arg>(arg)...} {}

    template<typename Arg1, typename... Arg2>
    CompressionPairImpl(OneThenVariadicArgs, Arg1&& arg1, Arg2&&... arg2)
        : first_(std::forward<Arg1>(arg1)), second_type{std::forward<Arg2>(arg2)...} {}

    first_reference first() { return first_; }
    first_const_reference first() const { return first_; }
    second_reference second() { return *this; }
    second_const_reference second() const { return *this; }

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

    template<typename... Arg>
    explicit CompressionPairImpl(Arg&&... arg) : first_type{std::forward<Arg>(arg)...} {}

    template<typename... Arg>
    explicit CompressionPairImpl(ZeroThenVariadicArgs, Arg&&... arg) : second_type{std::forward<Arg>(arg)...} {}

    template<typename Arg1, typename... Arg2>
    CompressionPairImpl(OneThenVariadicArgs, Arg1&& arg1, Arg2&&... arg2)
        : first_type(std::forward<Arg1>(arg1)), second_type{std::forward<Arg2>(arg2)...} {}

    first_reference first() { return *this; }
    first_const_reference first() const { return *this; }
    second_reference second() { return *this; }
    second_const_reference second() const { return *this; }
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

    template<typename... Arg>
    explicit CompressionPairImpl(Arg&&... arg) : first_{std::forward<Arg>(arg)...}, second_(first_) {}

    template<typename Arg1, typename... Arg2>
    CompressionPairImpl(OneThenVariadicArgs, Arg1&& arg1, Arg2&&... arg2)
        : first_(std::forward<Arg1>(arg1)), second_{std::forward<Arg2>(arg2)...} {}

    first_reference first() { return first_; }
    first_const_reference first() const { return first_; }
    second_reference second() { return second_; }
    second_const_reference second() const { return second_; }

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

    template<typename... Arg>
    explicit CompressionPairImpl(Arg&&... arg) : first_type{std::forward<Arg>(arg)...}, second_(static_cast<first_type>(*this)) {}

    template<typename Arg1, typename... Arg2>
    CompressionPairImpl(OneThenVariadicArgs, Arg1&& arg1, Arg2&&... arg2)
        : first_type(std::forward<Arg1>(arg1)), second_{std::forward<Arg2>(arg2)...} {}

    first_reference first() { return *this; }
    first_const_reference first() const { return *this; }
    second_reference second() { return second_; }
    second_const_reference second() const { return second_; }

private:
    second_type second_;
};

}

template<typename T1, typename T2, bool = std::is_same_v<T1, T2>>
class CompressionPair : private details::CompressionPairImpl<T1, T2, details::CompressionPairSwitch<T1, T2>::value>
{
    using base = details::CompressionPairImpl<T1, T2, details::CompressionPairSwitch<T1, T2>::value>;
public:
    using first_type             = base::first_type;
    using second_type            = base::second_type;
    using first_reference        = base::first_reference;
    using second_reference       = base::second_reference;
    using first_const_reference  = base::first_const_reference;
    using second_const_reference = base::second_const_reference;
    using first_param_type       = base::first_param_type;
    using second_param_type      = base::second_param_type;

    CompressionPair() : base() {}

    template<typename... Arg>
    explicit CompressionPair(Arg&&... arg) : base(std::forward<Arg>(arg)...) {}

    template<typename... Arg>
    explicit CompressionPair(ZeroThenVariadicArgs tag, Arg&&... arg) : base(tag, std::forward<Arg>(arg)...) {}

    template<typename Arg1, typename... Arg2>
    CompressionPair(OneThenVariadicArgs tag, Arg1&& arg1, Arg2&&... arg2)
        : base(tag, std::forward<Arg1>(arg1), std::forward<Arg2>(arg2)...) {}

    using base::first;
    using base::second;
};

template<typename T1, typename T2>
class CompressionPair<T1, T2, true> : private details::CompressionPairImpl<T1, T2, details::CompressionPairSwitch<T1, T2>::value>
{
    using base = details::CompressionPairImpl<T1, T2, details::CompressionPairSwitch<T1, T2>::value>;
public:
    using first_type             = base::first_type;
    using second_type            = base::second_type;
    using first_reference        = base::first_reference;
    using second_reference       = base::second_reference;
    using first_const_reference  = base::first_const_reference;
    using second_const_reference = base::second_const_reference;
    using first_param_type       = base::first_param_type;
    using second_param_type      = base::second_param_type;

    CompressionPair() : base() {}

    template<typename... Arg>
    explicit CompressionPair(Arg&&... arg) : base(std::forward<Arg>(arg)...) {}

    template<typename Arg1, typename... Arg2>
    CompressionPair(OneThenVariadicArgs tag, Arg1&& arg1, Arg2&&... arg2)
        : base(tag, std::forward<Arg1>(arg1), std::forward<Arg2>(arg2)...) {}

    using base::first;
    using base::second;
};

}