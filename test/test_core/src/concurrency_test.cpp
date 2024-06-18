// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "gtest/gtest.h"

#include "concurrency/lock_free_list.hpp"
#include "concurrency/priority_queue.hpp"
#include "io/llio.hpp"

namespace atlas
{

TEST(ConcurrencyTest, LockFreeListTest)
{
    FLockFreePointerFIFOBase<int32, 8> queue;
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

TEST(ConcurrencyTest, IOTest)
{
    static LowLevelIO llio;
    auto file = Path("C:\\Code\\atlas\\test\\test_core\\test.txt").normalize();

    {
        IOBuffer buffer = {'a','b','c','d','e'};

        auto task = llio.async_write(file, buffer);
        task.then([=](auto write) {
            EXPECT_TRUE(write == 5);

            auto read_task = llio.async_read(file);
            auto&& b =  read_task.get_result();
            EXPECT_TRUE(b.size() == 5);
        });
    }
}

}// namespace atlas