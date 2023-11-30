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
    ptr = standard_malloc.Realloc(ptr, 50);
    standard_malloc.Free(ptr);
    ptr = nullptr;

    ptr = standard_malloc.AlignedMalloc(100, 32);
    ptr = standard_malloc.AlignedRealloc(ptr, 50, 32);
    standard_malloc.AlignedFree(ptr);
}

}
}