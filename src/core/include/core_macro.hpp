// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#define INDEX_NONE (-1)
#define INDEX_NONE_ZU ((size_t)-1)

#define DO_NOT_USE_DIRECTLY

#ifdef __cpp_exceptions
#define CPP_EXCEPTIONS 1
#else
#define CPP_EXCEPTIONS 0
#endif

#ifndef FORWARD_DECLARE_OBJC_CLASS
#ifdef __OBJC__
#define FORWARD_DECLARE_OBJC_CLASS(classname) @class classname
#else
#define FORWARD_DECLARE_OBJC_CLASS(classname) class classname
#endif
#endif

#define ENUM_BIT_MASK(enum_type)                                        \
inline constexpr enum_type operator&(enum_type lhs, enum_type rhs) {    \
    return static_cast<enum_type>(                                      \
        static_cast<std::underlying_type_t<enum_type>>(lhs) &           \
        static_cast<std::underlying_type_t<enum_type>>(rhs));           \
}                                                                       \
inline constexpr enum_type operator|(enum_type lhs, enum_type rhs) {    \
    return static_cast<enum_type>(                                      \
        static_cast<std::underlying_type_t<enum_type>>(lhs) |           \
        static_cast<std::underlying_type_t<enum_type>>(rhs));           \
}                                                                       \
inline constexpr enum_type& operator&=(enum_type& lhs, enum_type rhs) { \
    lhs = lhs & rhs;                                                    \
    return lhs;                                                         \
}                                                                       \
inline constexpr enum_type& operator|=(enum_type& lhs, enum_type rhs) { \
    lhs = lhs | rhs;                                                    \
    return lhs;                                                         \
}