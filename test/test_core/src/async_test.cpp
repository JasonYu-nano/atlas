// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "gtest/gtest.h"

#include "async/schedule_on.hpp"
#include "async/task.hpp"

namespace atlas
{

AsyncScheduler g_async_scheduler;

void debug(const char* fmt)
{
    std::cout<<"thread ["<<std::this_thread::get_id()<<"] "<<fmt;
}

Task<> async_get_id2()
{
    debug("start async_get_id2\n");
    co_await schedule_on(g_async_scheduler);
    debug("resume async_get_id2\n");
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(10ms);
    co_return;
}

Task<int32> async_get_id(int32 v)
{
    debug("start async_get_id\n");
    co_await async_get_id2();
    debug("resume async_get_id\n");
    co_return v;
}

TEST(AsyncTest, CoroTest)
{
    debug("before async_get_id\n");
    Task<int32> t = async_get_id(1);
    debug("after async_get_id\n");
    t.then([](auto i) {
        debug("got async_get_id\n");
        EXPECT_EQ(i, 1);
    });
}

}// namespace atlas