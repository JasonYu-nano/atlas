// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "gtest/gtest.h"

#include "memory/standard_malloc.hpp"

namespace atlas
{
namespace test
{

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
    EXPECT_TRUE(ptr != nullptr);

    ptr = standard_malloc.AlignedRealloc(ptr, 128, 32);
    EXPECT_TRUE(ptr != nullptr);

    standard_malloc.AlignedFree(ptr);
}

}
}