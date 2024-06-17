// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <utility>
#include <coroutine>
#include <future>

namespace atlas
{

template <class T>
concept Scheduler = requires{ typename T::awaiter_type; };

/**
 * @brief Standard awaiter for scheduler.
 * Forwards a coroutine handle to scheduler directly.
 * @tparam Scheduler
 */
template<typename Scheduler, typename... Args>
class ScheduleAwaiter
{
    using payload_type = std::tuple<Args...>;
public:
    explicit ScheduleAwaiter(Scheduler& scheduler, const Args&... args) noexcept
        : scheduler_(&scheduler)
        , payload_(args...)
    {}

    explicit ScheduleAwaiter(Scheduler& scheduler, Args&&... args) noexcept
        : scheduler_(&scheduler)
        , payload_(std::move(args)...)
    {}

    ScheduleAwaiter(ScheduleAwaiter&& rhs) noexcept
        : scheduler_(std::exchange(rhs.scheduler_, nullptr))
        , payload_(std::move(rhs.payload_))
    {}

    ScheduleAwaiter(const ScheduleAwaiter&) = delete;
    ScheduleAwaiter& operator=(const ScheduleAwaiter&) = delete;

    constexpr bool await_ready() const noexcept
    {
        return false;
    }

    void await_suspend(std::coroutine_handle<> handle) noexcept
    {
        scheduler_ ? invoke_impl(handle, std::make_index_sequence<std::tuple_size_v<payload_type>>{}) : handle.resume();
    }

    void await_resume() noexcept {}

private:
    template<size_t... Indices>
    void invoke_impl(std::coroutine_handle<> handle, std::index_sequence<Indices...>)
    {
        auto ptr = &Scheduler::schedule;
        std::invoke(ptr, *scheduler_, handle, std::get<Indices>(payload_)...);
    }

    Scheduler* scheduler_;
    payload_type payload_;
};

template<typename Scheduler>
class ScheduleAwaiter<Scheduler>
{
public:
    explicit ScheduleAwaiter(Scheduler& scheduler) noexcept : scheduler_(&scheduler) {}

    ScheduleAwaiter(ScheduleAwaiter&& rhs) noexcept : scheduler_(std::exchange(rhs.scheduler_, nullptr)) {}

    ScheduleAwaiter(const ScheduleAwaiter&) = delete;
    ScheduleAwaiter& operator=(const ScheduleAwaiter&) = delete;

    constexpr bool await_ready() const noexcept
    {
        return false;
    }

    void await_suspend(std::coroutine_handle<> handle) noexcept
    {
        scheduler_ ? scheduler_->schedule(handle) : handle.resume();
    }

    void await_resume() noexcept {}

private:
    Scheduler* scheduler_;
};

/**
 * @brief AsyncScheduler will dispatch coroutine execution to thread pool by std::async
 */
class AsyncScheduler
{
public:
    using awaiter_type = ScheduleAwaiter<AsyncScheduler>;

    void schedule(std::coroutine_handle<> handle)
    {
        f_ = std::async([=]() {
            handle.resume();
        });
    }
private:
    std::future<void> f_;
};

/**
 * @brief Transfer execution to the scheduler after called this function.
 * @tparam S
 * @param scheduler
 * @param args Variadic that will be pass to scheduler
 * @return 
 */
template<Scheduler S, typename... Args>
typename S::awaiter_type co_schedule_on(S& scheduler, Args&&... args)
{
    using schedule_awaiter = typename S::awaiter_type;
    return schedule_awaiter(scheduler, std::forward<Args>(args)...);
}

}// namespace atlas
