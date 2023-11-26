//  Copyright(c) 2023-present, Atlas.
//  Distributed under the MIT License (http://opensource.org/licenses/MIT)

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
};

typedef MacPlatformTypes PlatformTypes;
}

#ifdef DLL_EXPORT
#undef DLL_EXPORT
#endif

#define DLL_EXPORT __attribute__((visibility("default")))
#define DLL_IMPORT