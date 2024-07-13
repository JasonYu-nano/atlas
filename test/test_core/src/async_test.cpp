// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "async/schedule_on.hpp"
#include "async/static_thread_pool.hpp"
#include "async/task.hpp"
#include "gtest/gtest.h"

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

TEST(AsyncTest, Coroutine)
{
    debug("before async_get_id\n");
    Task<int32> t = launch(async_get_id(1));
    debug("after async_get_id\n");
    t.then([](auto i) {
        debug("got async_get_id\n");
        EXPECT_EQ(i, 1);
    });
}

TEST(AsyncTest, TaskCancellationPropagation)
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

TEST(AsyncTest, CancellationTask)
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

TEST(AsyncTest, StaticThreadPool)
{
    StaticThreadPool<2> thread_pool(2);

    std::condition_variable cv, cv_task;
    std::mutex mutex, mutex_task;
    int32 i = 0;

    thread_pool.push_task(1, [&]() {
        std::unique_lock lock(mutex_task);
        cv_task.wait(lock);
        i += 1;
        cv.notify_all();
    });

    thread_pool.push_task(0, [&]() {
        std::this_thread::sleep_for(2ms);
        i += 2;
        cv_task.notify_all();
    });

    std::unique_lock lock(mutex);
    cv.wait(lock);
    EXPECT_TRUE(i == 3);
}

}// namespace atlas