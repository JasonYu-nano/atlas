// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <type_traits>

namespace atlas
{

namespace details
{

template<typename T, bool Small>
struct CallTraitParamTypeImpl
{
    using Type = const T&;
};

template<typename T>
struct CallTraitParamTypeImpl<T, true>
{
    using Type = const T;
};

template<typename T, bool IsPointer, bool IsArithmetic, bool IsEnum>
struct CallTraitParamType
{
    using Type = const T&;
};

template<typename T, bool IsPointer, bool IsArithmetic>
struct CallTraitParamType<T, IsPointer, IsArithmetic, true>
{
    using Type = CallTraitParamTypeImpl<T, sizeof(T) <= sizeof(uintptr_t)>::Type;
};

template<typename T, bool IsPointer, bool IsEnum>
struct CallTraitParamType<T, IsPointer, true, IsEnum>
{
    using Type = CallTraitParamTypeImpl<T, sizeof(T) <= sizeof(uintptr_t)>::Type;
};

template<typename T, bool IsArithmetic, bool IsEnum>
struct CallTraitParamType<T, true, IsArithmetic, IsEnum>
{
    using Type = const T;
};

}

/**
 * see boost::call_traits
 * @tparam T
 */
template <typename T>
struct CallTraits
{
    using ValueType         = T;
    using Reference         = T&;
    using ConstReference    = const T&;
    using ParamType         = typename details::CallTraitParamType<T, std::is_pointer_v<T>, std::is_arithmetic_v<T>, std::is_enum_v<T>>::Type;
};

template <typename T>
struct CallTraits<T&>
{
    using ValueType         = T&;
    using Reference         = T&;
    using ConstReference    = const T&;
    using ParamType         = T&;
};

template <typename T>
struct CallTraits<const T&>
{
    using ValueType         = T&;
    using Reference         = T&;
    using ConstReference    = const T&;
    using ParamType         = T&;
};

template <typename T>
struct CallTraits<volatile T&>
{
    using ValueType         = T&;
    using Reference         = T&;
    using ConstReference    = const T&;
    using ParamType         = T&;
};

template <typename T>
struct CallTraits<const volatile T&>
{
    using ValueType         = T&;
    using Reference         = T&;
    using ConstReference    = const T&;
    using ParamType         = T&;
};

template <typename T>
struct CallTraits<T*>
{
    using ValueType         = T*;
    using Reference         = T*&;
    using ConstReference    = T* const;
    using ParamType         = T* const;
};

template <typename T, size_t N>
struct CallTraits<T[N]>
{
private:
    typedef T ArrayType[N];
public:
    using ValueType         = const T*;
    using Reference         = ArrayType&;
    using ConstReference    = const ArrayType&;
    using ParamType         = const T* const;
};

template <typename T, size_t N>
struct CallTraits<const T[N]>
{
private:
    typedef const T ArrayType[N];
public:
    using ValueType         = const T*;
    using Reference         = ArrayType&;
    using ConstReference    = const ArrayType&;
    using ParamType         = const T* const;
};

}