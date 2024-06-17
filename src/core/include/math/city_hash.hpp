// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <utility>

#include "core_def.hpp"

namespace atlas::city_hash
{

typedef std::pair<uint64, uint64> uint128;

inline uint64 uint128_low64(const uint128& x) { return x.first; }
inline uint64 uint128_high64(const uint128& x) { return x.second; }

// Hash function for a byte array.
uint64 city_hash64(const char *buf, size_t len);

// Hash function for a byte array.  For convenience, a 64-bit seed is also
// hashed into the result.
uint64 city_hash64_with_seed(const char *buf, size_t len, uint64 seed);

// Hash function for a byte array.  For convenience, two seeds are also
// hashed into the result.
uint64 city_hash64_with_seeds(const char *buf, size_t len,
                           uint64 seed0, uint64 seed1);

// Hash function for a byte array.
uint128 city_hash128(const char *s, size_t len);

// Hash function for a byte array.  For convenience, a 128-bit seed is also
// hashed into the result.
uint128 city_hash128_with_seed(const char *s, size_t len, uint128 seed);

// Hash function for a byte array.  Most useful in 32-bit binaries.
uint32 city_hash32(const char *buf, size_t len);

// Hash 128 input bits down to 64 bits of output.
// This is intended to be a reasonably good hash function.
inline uint64 hash128_to64(const uint128& x)
{
    // Murmur-inspired hashing.
    const uint64 kMul = 0x9ddfea08eb382d69ULL;
    uint64 a = (uint128_low64(x) ^ uint128_high64(x)) * kMul;
    a ^= (a >> 47);
    uint64 b = (uint128_high64(x) ^ a) * kMul;
    b ^= (b >> 47);
    b *= kMul;
    return b;
}

} // namespace atlas::city_hash
