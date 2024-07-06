// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <future>
#include <coroutine>
#include <utility>

#include "core_log.hpp"
#include "misc/delegate_fwd.hpp"
#include "misc/stop_token.hpp"


namespace atlas
{

template <typename T>
concept Awaitable = requires(T t, std::coroutine_handle<> h)
{
    { t.await_ready() } -> std::convertible_to<bool>;
    { t.await_suspend(h) };
    { t.await_resume() };
};

template<typename T = void>
class TaskBase;

template<typename T = void>
class Task;

namespace details
{

struct SharedCoroutineReferenceCounter
{
    std::atomic<uint32> uses = 1;
};

class SharedCoroutineHandle
{
public:
    SharedCoroutineHandle() = default;

    explicit SharedCoroutineHandle(std::nullptr_t) {}

    explicit SharedCoroutineHandle(std::coroutine_handle<> handle)
        : handle_ptr_(handle.address())
        , counter_(new SharedCoroutineReferenceCounter())
    {}

    SharedCoroutineHandle(const SharedCoroutineHandle& rhs)
        : handle_ptr_(rhs.handle_ptr_)
        , counter_(rhs.counter_)
    {
        if (counter_)
        {
            counter_->uses.fetch_add(1, std::memory_order_relaxed);
        }
    }

    SharedCoroutineHandle(SharedCoroutineHandle&& rhs) noexcept
        : handle_ptr_(std::exchange(rhs.handle_ptr_, nullptr))
        , counter_(std::exchange(rhs.counter_, nullptr))
    {}

    ~SharedCoroutineHandle()
    {
        reset();
    }

    SharedCoroutineHandle& operator= (std::nullptr_t)
    {
        reset();
        return *this;
    }

    SharedCoroutineHandle& operator= (const SharedCoroutineHandle& rhs)
    {
        if (std::addressof(rhs) != this)
        {
            SharedCoroutineHandle{rhs}.swap(*this);
        }
        return *this;
    }

    SharedCoroutineHandle& operator= (SharedCoroutineHandle&& rhs) noexcept
    {
        if (std::addressof(rhs) != this)
        {
            SharedCoroutineHandle{std::move(rhs)}.swap(*this);
        }
        return *this;
    }

    void reset()
    {
        if (counter_)
        {
            const uint32 old = counter_->uses.fetch_sub(1, std::memory_order_acq_rel);
            if (old == 1)
            {
                destroy_handle();
            }
            else
            {
                handle_ptr_ = nullptr;
                counter_ = nullptr;
            }
        }
    }

    template<typename PromiseType>
    NODISCARD auto get() const
    {
        return handle_ptr_ ? std::coroutine_handle<PromiseType>::from_address(handle_ptr_) : nullptr;
    }

    void swap(SharedCoroutineHandle& rhs) noexcept
    {
        std::swap(handle_ptr_, rhs.handle_ptr_);
        std::swap(counter_, rhs.counter_);
    }

private:
    void destroy_handle()
    {
        void* handle = std::exchange(handle_ptr_, nullptr);
        auto counter = std::exchange(counter_, nullptr);
        delete counter;
        auto co_handle = std::coroutine_handle<>::from_address(handle);
        co_handle.destroy();
    }

    void* handle_ptr_{ nullptr };
    SharedCoroutineReferenceCounter* counter_{ nullptr };
};

template<typename PromiseType>
struct TaskAwaiter
{
    using handle_type = std::coroutine_handle<PromiseType>;
    explicit TaskAwaiter(handle_type handle) noexcept : handle_(handle) {}

    TaskAwaiter(TaskAwaiter&& completion) noexcept : handle_(std::exchange(completion.handle_, nullptr)) {}

    TaskAwaiter(const TaskAwaiter&) = delete;

    TaskAwaiter& operator=(const TaskAwaiter&) = delete;

    constexpr bool await_ready() const noexcept
    {
        return false;
    }

    void await_suspend(std::coroutine_handle<> continuation) noexcept
    {
        handle_.promise().set_continuation(continuation);
        handle_.resume();
    }

    typename PromiseType::value_type await_resume() noexcept
    {
        if constexpr (!std::is_same_v<typename PromiseType::value_type, void>)
        {
            return handle_.promise().get_result();
        }
    }

private:
    std::coroutine_handle<PromiseType> handle_;
};

class TaskPromiseBase
{
    struct FinalAwaiter
    {
        constexpr bool await_ready() const noexcept { return false; }

        template<typename Promise>
        void await_suspend(std::coroutine_handle<Promise> coroutine) const noexcept
        {
            TaskPromiseBase& promise = coroutine.promise();
            auto continuation = std::exchange(promise.continuation_, nullptr);
            promise.shared_handle_.reset();
            if (continuation)
            {
                continuation.resume();
            }
        }

        void await_resume() const noexcept {}
    };
public:
    TaskPromiseBase() : shared_handle_(std::coroutine_handle<TaskPromiseBase>::from_promise(*this)) {}

    std::suspend_always initial_suspend() { return {}; }
    FinalAwaiter final_suspend() noexcept
    {
        return {};
    }

    void unhandled_exception()
    {
        LOG_CRITICAL(core, "Fatal exception in coroutine");
        std::terminate();
    }

    template<typename TaskType>
    auto await_transform(TaskType&& task) requires(std::is_base_of_v<TaskPromiseBase, typename TaskType::promise_type>)
    {
        StopToken local{ stop_token_ };
        task.set_stop_token(std::move(local));
        auto handle = task.co_handle();
        return TaskAwaiter<typename TaskType::promise_type>(handle);
    }

    template<Awaitable AwaiterType>
    AwaiterType await_transform(AwaiterType&& awaiter)
    {
        return awaiter;
    }

    void set_stop_token(const StopToken& token)
    {
        stop_token_ = token;
    }

    void set_stop_token(StopToken&& token)
    {
        stop_token_ = std::move(token);
    }

    NODISCARD const StopToken& get_stop_token() const
    {
        return stop_token_;
    }

    void set_continuation(std::coroutine_handle<> continuation)
    {
        continuation_ = continuation;
    }

protected:
    StopToken stop_token_;
    SharedCoroutineHandle shared_handle_;
    std::coroutine_handle<> continuation_{ nullptr };
};

template<typename T>
class TaskPromise : public TaskPromiseBase
{
    using base = TaskPromiseBase;
    using base::shared_handle_;
public:
    using value_type = T;

    TaskPromise() = default;

    Task<T> get_return_object()
    {
        return Task<T>(shared_handle_);
    }

    void return_value(T value)
    {
        std::unique_lock lock(mutex_);
        value_ = std::move(value);
        completion_.notify_all();
        if (on_completed_.is_empty() || stop_token_.stop_requested())
        {
            return;
        }

        InlineArray<std::function<void(T)>, 10> invocation = std::move(on_completed_);
        lock.unlock();
        for (auto&& fn : invocation)
        {
            fn(*value_);
        }
    }

    T& get_result()
    {
        std::unique_lock lock(mutex_);
        if (!value_.has_value())
        {
            completion_.wait(lock);
        }
        return *value_;
    }

    void on_completed(std::function<void(T)>&& func)
    {
        std::unique_lock lock(mutex_);
        if (!stop_token_.stop_requested())
        {
            if (value_.has_value())
            {
                lock.unlock();
                func(*value_);
            }
            else
            {
                on_completed_.add(std::move(func));
            }
        }
    }

private:
    std::mutex mutex_;
    std::condition_variable completion_;
    std::optional<T> value_;
    InlineArray<std::function<void(T)>, 3> on_completed_;
};

template<>
class TaskPromise<void> : public TaskPromiseBase
{
    using base = TaskPromiseBase;
public:
    using value_type = void;

    TaskPromise() = default;

    Task<> get_return_object();

    void return_void()
    {
        std::unique_lock lock(mutex_);
        has_value_ = true;
        completion_.notify_all();
        if (on_completed_.is_empty() || stop_token_.stop_requested())
        {
            return;
        }

        InlineArray<std::function<void()>, 10> invocation = std::move(on_completed_);
        lock.unlock();
        for (auto&& fn : invocation)
        {
            fn();
        }
    }

    void wait()
    {
        std::unique_lock lock(mutex_);
        if (!has_value_)
        {
            completion_.wait(lock);
        }
    }

    void on_completed(std::function<void()>&& func)
    {
        std::unique_lock lock(mutex_);
        if (!stop_token_.stop_requested())
        {
            if (has_value_)
            {
                lock.unlock();
                func();
            }
            else
            {
                on_completed_.add(std::move(func));
            }
        }
    }

private:
    std::mutex mutex_;
    std::condition_variable completion_;
    bool has_value_{ false };
    InlineArray<std::function<void()>, 3> on_completed_;
};

}// namespace details

template<typename T>
class TaskBase
{
protected:
    using shared_coroutine_handle = details::SharedCoroutineHandle;
public:
    using promise_type = details::TaskPromise<T>;

    TaskBase(TaskBase&& task) noexcept: shared_handle_(std::exchange(task.shared_handle_, {})) {}

    TaskBase(TaskBase&) = delete;

    TaskBase& operator=(TaskBase&) = delete;

    ~TaskBase() = default;

    NODISCARD std::coroutine_handle<promise_type> co_handle() const
    {
        return shared_handle_.get<promise_type>();
    }

    void start()
    {
        auto handle = co_handle();
        if (!handle.done())
        {
            handle.resume();
        }
    }

    void set_stop_token(const StopToken& token)
    {
        auto handle = co_handle();
        handle.promise().set_stop_token(token);
    }

    void set_stop_token(StopToken&& token)
    {
        auto handle = co_handle();
        handle.promise().set_stop_token(std::move(token));
    }

protected:
    explicit TaskBase(const shared_coroutine_handle& handle) noexcept: shared_handle_(handle) {}

    shared_coroutine_handle shared_handle_;
};

/**
 * @brief A task represents an operation that produces a result both lazily and asynchronously.
 * When you call a coroutine that returns a task, the coroutine will suspend immediately until call co_await or
 * start or launch to resume coroutine.
 * To stop a task, you should use launch to pass a stop token to coroutine.
 * @note The coroutine handle will only be destroyed when execution completed and a relevant task destructs.
 * So it's safe to invoke callbacks after a task destructed.
 * @tparam T
 */
template<typename T>
class Task : public TaskBase<T>
{
    using base = TaskBase<T>;
public:
    using base::co_handle;

    friend class details::TaskPromise<T>;

    Task(Task&& task) noexcept : base(std::move(task)) {}

    Task(Task&) = delete;

    Task& operator=(Task&) = delete;

    ~Task() = default;

    /**
     * @brief Wait task completes, then return the coroutine result.
     * @return
     */
    T& get_result()
    {
        return co_handle().promise().get_result();
    }

    /**
     * @brief Adds callback to invoke when the task completed.
     * @note Callback won't be executed when the task was canceled.
     * @param func
     * @return
     */
    Task& then(std::function<void(T)>&& func)
    {
        co_handle().promise().on_completed(std::move(func));
        return *this;
    }
    /**
     * @brief Adds callback to invoke when the task completed.
     * @note Callback won't be executed when the task was canceled.
     * @param func
     * @return
     */
    Task& then(std::function<void()>&& func)
    {
        co_handle().promise().on_completed([func](auto v) {
            func();
        });
        return *this;
    }

private:
    explicit Task(const base::shared_coroutine_handle& handle) noexcept : base(handle) {}
};

/**
 * @brief A task represents an operation that produces a result both lazily and asynchronously.
 * When you call a coroutine that returns a task, the coroutine will suspend immediately until call co_await or
 * start or launch to resume coroutine.
 * To stop a task, you should use launch to pass a stop token to coroutine.
 * @note The coroutine handle will only be destroyed when execution completed and a relevant task destructs.
 * So it's safe to invoke callbacks after a task destructed.
 */
template<>
class Task<void> : public TaskBase<>
{
    using base = TaskBase;
public:
    friend class details::TaskPromise<void>;
    Task(Task&& task) noexcept : base(std::move(task)) {}

    Task(Task&) = delete;

    Task& operator=(Task&) = delete;

    ~Task() = default;
    /**
    * @brief Waits task complete.
    */
    void wait()
    {
        co_handle().promise().wait();
    }
    /**
     * @brief Adds callback to invoke when the task completed.
     * @note Callback won't be executed when the task was canceled.
     * @param func
     * @return
     */
    Task& then(std::function<void()>&& func)
    {
        co_handle().promise().on_completed(std::move(func));
        return *this;
    }

private:
    explicit Task(const shared_coroutine_handle& handle) noexcept : base(handle) {}
};

inline Task<> details::TaskPromise<void>::get_return_object()
{
    return Task{shared_handle_};
}

class StopTokenAwaiter
{

public:
    StopTokenAwaiter() = default;

    StopTokenAwaiter(StopTokenAwaiter&& rhs) = default;

    StopTokenAwaiter(const StopTokenAwaiter&) = delete;
    StopTokenAwaiter& operator=(const StopTokenAwaiter&) = delete;

    constexpr bool await_ready() const noexcept
    {
        return false;
    }

    template<typename PromiseType>
    void await_suspend(std::coroutine_handle<PromiseType> handle) noexcept
    {
        token_ = handle.promise().get_stop_token();
        handle.resume();
    }

    StopToken await_resume() const noexcept
    {
        return token_;
    }
private:
    StopToken token_;
};

StopTokenAwaiter inline co_get_current_stop_token()
{
    return {};
}

/**
 * @brief Starts the task.
 * @tparam T
 * @param task
 * @return
 */
template<typename T>
auto launch(Task<T>&& task)
{
    task.start();
    return task;
}
/**
 * @brief Passes stop token to the task and then start it.
 * @tparam T
 * @param task
 * @param token
 * @return
 */
template<typename T>
auto launch(Task<T>&& task, StopToken&& token)
{
    task.set_stop_token(std::move(token));
    task.start();
    return task;
}
/**
 * @brief Passes stop token to the task and then start it.
 * @tparam T
 * @param task
 * @param token
 * @return
 */
template<typename T>
auto launch(Task<T>&& task, const StopToken& token)
{
    task.set_stop_token(token);
    task.start();
    return task;
}

}// namespace atlas