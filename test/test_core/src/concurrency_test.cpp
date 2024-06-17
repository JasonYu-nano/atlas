// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "gtest/gtest.h"

#include "concurrency/lock_free_list.hpp"
#include "concurrency/priority_queue.hpp"

namespace atlas
{

TEST(ConcurrencyTest, LockFreeListTest)
{
    LockFreePointerFIFOBase<int32, 8> queue;
    queue.push(new int32(1));
    int32* t = queue.pop();
    EXPECT_EQ(*t, 1);
    delete t;
}

TEST(ConcurrencyTest, PriorityQueueTest)
{
    PriorityQueue<int32, 3> queue;
    queue.push(new int32(1), 1);
    queue.push(new int32(2), 0);
    int32* t = queue.pop(1, false);
    EXPECT_EQ(*t, 2);
    delete t;
}

}// namespace atlas