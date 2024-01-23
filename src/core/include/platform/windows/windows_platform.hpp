// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

namespace atlas
{
struct WindowsPlatformTypes
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

typedef WindowsPlatformTypes PlatformTypes;
}

#undef DLL_EXPORT

#ifdef AE_SHARED
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#undef DLL_IMPORT

#ifdef AE_SHARED
#define DLL_IMPORT __declspec(dllimport)
#else
#define DLL_IMPORT
#endif

#undef NODISCARD
#define NODISCARD [[nodiscard]]
