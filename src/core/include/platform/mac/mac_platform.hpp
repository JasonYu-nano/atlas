// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once
#include "core_def.hpp"

namespace atlas
{
struct MacPlatformTypes
{
    typedef unsigned long long      uint64;
    typedef unsigned int            uint32;
    typedef unsigned short int      uint16;
    typedef unsigned char           uint8;

    typedef signed long long        int64;
    typedef signed int              int32;
    typedef signed short int        int16;
    typedef signed char             int8;

    typedef unsigned char           byte;

    typedef uint64                  size_t;
    typedef double                  real_t;
};

typedef MacPlatformTypes PlatformTypes;
}

#undef DLL_EXPORT

#ifdef AE_SHARED
#define DLL_EXPORT __attribute__((visibility("default")))
#else
#define DLL_EXPORT
#endif

#undef DLL_IMPORT
#define DLL_IMPORT

#undef NODISCARD
#define NODISCARD [[nodiscard]]

#define PLATFORM_CACHE_LINE_SIZE	64