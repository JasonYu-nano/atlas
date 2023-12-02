// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "gtest/gtest.h"

#include "memory/standard_malloc.hpp"

namespace atlas::test
{

struct alignas(32) Alignment32
{
    bool flag;
};

TEST(StandardMallocTest, MallocTest)
{
    StandardMalloc standard_malloc;

    void* ptr = standard_malloc.Malloc(100);
    EXPECT_TRUE(ptr != nullptr);

    ptr = standard_malloc.Realloc(ptr, 50);
    EXPECT_TRUE(ptr != nullptr);

    standard_malloc.Free(ptr);
    ptr = nullptr;

    ptr = standard_malloc.AlignedMalloc(64, 32);
    EXPECT_TRUE(ptr != nullptr && reinterpret_cast<uintptr_t>(ptr) % 32 == 0);

    ptr = standard_malloc.AlignedRealloc(ptr, 128, 32);
    EXPECT_TRUE(ptr != nullptr && reinterpret_cast<uintptr_t>(ptr) % 32 == 0);

    standard_malloc.AlignedFree(ptr);
}

TEST(OperatorNewDeleteTest, MallocTest)
{
    // aligned new
    auto al = new Alignment32();
    EXPECT_TRUE(al != nullptr && reinterpret_cast<uintptr_t>(al) % 32 == 0);

    // aligned delete
    delete al;

    // aligned new[]
    auto al_array = new Alignment32[6]();
    EXPECT_TRUE(al_array != nullptr &&
        reinterpret_cast<uintptr_t>(al) % 32 == 0 &&
        reinterpret_cast<uintptr_t>(al_array + 1) % 32 == 0);

    // aligned delete[]
    delete[] al_array;

    // aligned no throw new
    auto al_nothrow = new(std::nothrow) Alignment32();
    EXPECT_TRUE(al_array != nullptr && reinterpret_cast<uintptr_t>(al) % 32 == 0);

    delete(al_nothrow);
}

}
