// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "gtest/gtest.h"

#include "async/schedule_on.hpp"
#include "async/task.hpp"

namespace atlas
{

using namespace std::chrono_literals;

AsyncScheduler g_async_scheduler;

void debug(const char* fmt)
{
    std::cout<<"thread ["<<std::this_thread::get_id()<<"] "<<fmt;
}

Task<> async_get_id2()
{
    debug("start async_get_id2\n");
    co_await co_schedule_on(g_async_scheduler);
    debug("resume async_get_id2\n");
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
    Task<int32> t = launch(async_get_id(1));
    debug("after async_get_id\n");
    t.then([](auto i) {
        debug("got async_get_id\n");
        EXPECT_EQ(i, 1);
    });
}

TEST(AsyncTest, CancellationPropagationTest)
{
    StopSource source;
    StopToken token = source.get_token();
    auto task = launch([&]() -> Task<> {
        co_await [&]() -> Task<> {
            auto token2 = co_await co_get_current_stop_token();
            EXPECT_TRUE(token == token2);
            co_return;
        }();
        co_return;
    }(), token);
}

TEST(AsyncTest, CancellationTest)
{
    StopSource source;
    auto task = launch([&]() -> Task<> {
        StopToken token = co_await co_get_current_stop_token();
        EXPECT_FALSE(token.stop_requested());
        co_await co_schedule_on(g_async_scheduler);
        std::this_thread::sleep_for(50ms);
        EXPECT_TRUE(token.stop_requested());
        co_return;
    }(), source.get_token());

    task.then([](){
        EXPECT_TRUE(false); // callback won't be executed after cancellation.
    });

    std::this_thread::sleep_for(10ms);

    source.request_stop();
    task.wait();
}

}// namespace atlas