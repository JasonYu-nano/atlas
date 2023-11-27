//  Copyright(c) 2023-present, Atlas.
//  Distributed under the MIT License (http://opensource.org/licenses/MIT)

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
};

typedef WindowsPlatformTypes PlatformTypes;
}

#ifdef DLL_EXPORT
#undef DLL_EXPORT
#endif

#define DLL_EXPORT __declspec(dllexport)

#ifdef DLL_IMPORT
#undef DLL_IMPORT
#endif

#define DLL_IMPORT __declspec(dllimport)