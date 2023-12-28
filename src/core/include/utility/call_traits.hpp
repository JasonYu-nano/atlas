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
    using type = const T&;
};

template<typename T>
struct CallTraitParamTypeImpl<T, true>
{
    using type = const T;
};

template<typename T, bool IsPointer, bool IsArithmetic, bool IsEnum>
struct CallTraitParamType
{
    using type = const T&;
};

template<typename T, bool IsPointer, bool IsArithmetic>
struct CallTraitParamType<T, IsPointer, IsArithmetic, true>
{
    using type = CallTraitParamTypeImpl<T, sizeof(T) <= sizeof(uintptr_t)>::type;
};

template<typename T, bool IsPointer, bool IsEnum>
struct CallTraitParamType<T, IsPointer, true, IsEnum>
{
    using type = CallTraitParamTypeImpl<T, sizeof(T) <= sizeof(uintptr_t)>::type;
};

template<typename T, bool IsArithmetic, bool IsEnum>
struct CallTraitParamType<T, true, IsArithmetic, IsEnum>
{
    using type = const T;
};

}

/**
 * see boost::call_traits
 * @tparam T
 */
template <typename T>
struct CallTraits
{
    using value_type         = T;
    using reference          = T&;
    using const_reference    = const T&;
    using param_type         = typename details::CallTraitParamType<T, std::is_pointer_v<T>, std::is_arithmetic_v<T>, std::is_enum_v<T>>::type;
};

template <typename T>
struct CallTraits<T&>
{
    using value_type         = T&;
    using reference          = T&;
    using const_reference    = const T&;
    using param_type         = T&;
};

template <typename T>
struct CallTraits<const T&>
{
    using value_type         = T&;
    using reference          = T&;
    using const_reference    = const T&;
    using param_type         = T&;
};

template <typename T>
struct CallTraits<volatile T&>
{
    using value_type         = T&;
    using reference          = T&;
    using const_reference    = const T&;
    using param_type         = T&;
};

template <typename T>
struct CallTraits<const volatile T&>
{
    using value_type         = T&;
    using reference          = T&;
    using const_reference    = const T&;
    using param_type         = T&;
};

template <typename T>
struct CallTraits<T*>
{
    using value_type         = T*;
    using reference          = T*&;
    using const_reference    = T* const;
    using param_type         = T* const;
};

template <typename T, size_t N>
struct CallTraits<T[N]>
{
private:
    typedef T ArrayType[N];
public:
    using value_type         = const T*;
    using reference          = ArrayType&;
    using const_reference    = const ArrayType&;
    using param_type         = const T* const;
};

template <typename T, size_t N>
struct CallTraits<const T[N]>
{
private:
    typedef const T ArrayType[N];
public:
    using value_type         = const T*;
    using reference          = ArrayType&;
    using const_reference    = const ArrayType&;
    using param_type         = const T* const;
};

}