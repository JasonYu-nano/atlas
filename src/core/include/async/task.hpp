// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <future>
#include <coroutine>

#include "core_log.hpp"
#include "misc/delegate_fwd.hpp"


namespace atlas
{

namespace details
{

struct SharedCoroutineReferenceCounter
{
    std::atomic<uint32> uses = 1;
};

}

template<typename PromiseType>
class SharedCoroutineHandle
{
    using SharedCoroutineReferenceCounter = details::SharedCoroutineReferenceCounter;
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
        if (handle_ptr_)
        {
            ++counter_->uses;
        }
    }

    SharedCoroutineHandle(SharedCoroutineHandle&& rhs) noexcept
        : handle_ptr_(std::exchange(rhs.handle_ptr_, nullptr))
        , counter_(std::exchange(rhs.counter_, nullptr))
    {}

    ~SharedCoroutineHandle()
    {
        release();
    }

    SharedCoroutineHandle& operator= (std::nullptr_t)
    {
        release();
        return *this;
    }

    SharedCoroutineHandle& operator= (const SharedCoroutineHandle& rhs)
    {
        if (std::addressof(rhs) != this)
        {
            release();
            handle_ptr_ = rhs.handle_ptr_;
            counter_ = rhs.counter_;
        }
        return *this;
    }

    SharedCoroutineHandle& operator= (SharedCoroutineHandle&& rhs) noexcept
    {
        if (std::addressof(rhs) != this)
        {
            release();
            handle_ptr_ = std::exchange(rhs.handle_ptr_, nullptr);
            counter_ = std::exchange(rhs.counter_, nullptr);
        }
        return *this;
    }

    std::coroutine_handle<PromiseType> operator* () const
    {
        return get_handle();
    }

    void release()
    {
        if (handle_ptr_)
        {
            const uint32 old = counter_->uses.fetch_sub(1);
            if (old == 1)
            {
                destroy_handle();
            }
            else
            {
                handle_ptr_ = nullptr;
            }
        }
    }

    std::coroutine_handle<PromiseType> get_handle() const
    {
        return handle_ptr_ ? std::coroutine_handle<PromiseType>::from_address(handle_ptr_) : nullptr;
    }

private:
    void destroy_handle()
    {
        void* handle = std::exchange(handle_ptr_, nullptr);
        SharedCoroutineReferenceCounter* counter = std::exchange(counter_, nullptr);
        delete counter;
        auto co_handle = std::coroutine_handle<>::from_address(handle);
        co_handle.destroy();
    }

    void* handle_ptr_{ nullptr };
    SharedCoroutineReferenceCounter* counter_{ nullptr };
};

template<typename T = void>
class Task;

namespace details
{

template<typename PromiseType>
struct TaskAwaiter
{
    explicit TaskAwaiter(PromiseType& promise) noexcept : promise_(&promise) {}

    TaskAwaiter(TaskAwaiter&& completion) noexcept : promise_(std::exchange(completion.promise_, nullptr)) {}

    TaskAwaiter(TaskAwaiter&) = delete;

    TaskAwaiter& operator=(TaskAwaiter &) = delete;

    constexpr bool await_ready() const noexcept
    {
        return false;
    }

    void await_suspend(std::coroutine_handle<> handle) noexcept
    {
        if constexpr (std::is_same_v<typename PromiseType::value_type, void>)
        {
            promise_->on_completed([handle] {
                handle.resume();
            });
        }
        else
        {
            promise_->on_completed([handle](auto v) {
                handle.resume();
            });
        }
    }

    typename PromiseType::value_type await_resume() noexcept
    {
        if constexpr (!std::is_same_v<typename PromiseType::value_type, void>)
        {
            return promise_->get_result();
        }
    }

private:
    PromiseType* promise_;
};

template<typename T>
class TaskPromise
{
public:
    using value_type = T;

    TaskPromise() : shared_handle_(std::coroutine_handle<TaskPromise>::from_promise(*this)) {}

    std::suspend_never initial_suspend() { return {}; }
    std::suspend_always final_suspend() noexcept
    {
        shared_handle_.release();
        return {};
    }

    Task<T> get_return_object()
    {
        return Task<T>(shared_handle_);
    }

    void unhandled_exception()
    {
        LOG_CRITICAL(core, "Fatal exception in coroutine");
    }

    void return_value(T value)
    {
        do
        {
            std::unique_lock lock(mutex_);
            value_ = std::move(value);
            completion_.notify_all();
            if (on_completed_.IsEmpty())
            {
                lock.unlock();
                break;
            }

            InlineArray<std::function<void(T)>, 10> invocation = std::move(on_completed_);
            lock.unlock();
            for (auto&& fn : invocation)
            {
                fn(*value_);
            }
        }
        while (false);
    }

    template<typename RetType>
    TaskAwaiter<typename Task<RetType>::promise_type> await_transform(Task<RetType>&& task)
    {
        return TaskAwaiter<typename Task<RetType>::promise_type>(task.get_promise());
    }

    template<typename Awaiter>
    Awaiter await_transform(Awaiter&& awaiter)
    {
        return awaiter;
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
        if (value_.has_value())
        {
            lock.unlock();
            func(*value_);
        }
        else
        {
            on_completed_.Add(std::move(func));
        }
    }

private:
    std::mutex mutex_;
    std::condition_variable completion_;
    std::optional<T> value_;
    InlineArray<std::function<void(T)>, 3> on_completed_;
    SharedCoroutineHandle<TaskPromise> shared_handle_;
};

template<>
class TaskPromise<void>
{
public:
    using value_type = void;

    TaskPromise() : shared_handle_(std::coroutine_handle<TaskPromise>::from_promise(*this)) {}

    std::suspend_never initial_suspend() { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }

    Task<> get_return_object();

    void unhandled_exception()
    {
        LOG_CRITICAL(core, "Fatal exception in coroutine");
    }

    void return_void()
    {
        {
            std::unique_lock lock(mutex_);
            has_value_ = true;
            completion_.notify_all();
            InlineArray<std::function<void()>, 10> invocation = std::move(on_completed_);
            lock.unlock();
            for (auto&& fn : invocation)
            {
                fn();
            }
        }
        shared_handle_.release();
    }

    template<typename RetType>
    TaskAwaiter<typename Task<RetType>::promise_type> await_transform(Task<RetType>&& task)
    {
        return TaskAwaiter<typename Task<RetType>::promise_type>(task.get_promise());
    }

    template<typename Awaiter>
    Awaiter await_transform(Awaiter&& awaiter)
    {
        return awaiter;
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
        if (has_value_)
        {
            lock.unlock();
            func();
        }
        else
        {
            on_completed_.Add(std::move(func));
        }
    }

private:
    std::mutex mutex_;
    std::condition_variable completion_;
    bool has_value_{ false };
    InlineArray<std::function<void()>, 3> on_completed_;
    SharedCoroutineHandle<TaskPromise> shared_handle_;
};

}// namespace details

template<typename T>
class TaskBase
{
public:
    using promise_type = details::TaskPromise<T>;

    explicit TaskBase(const SharedCoroutineHandle<promise_type>& handle) noexcept: shared_handle_(handle) {}

    TaskBase(TaskBase&& task) noexcept: shared_handle_(std::exchange(task.shared_handle_, {})) {}

    TaskBase(TaskBase&) = delete;

    TaskBase& operator=(TaskBase&) = delete;

    ~TaskBase() = default;

    promise_type& get_promise()
    {
        return (*shared_handle_).promise();
    }
protected:
    SharedCoroutineHandle<promise_type> shared_handle_;
};

/**
 * @brief A task represents an operation that produces a result both lazily and asynchronously.
 * When you call a coroutine that returns a task, the coroutine will returns exeuction to the caller when it suspend or return.
 * @note The coroutine handle will only be destroyed when execution completed and relavent task destruct.
 * So it's safe to invoke callbacks after task destructed.
 * @tparam T
 */
template<typename T>
class Task : public TaskBase<T>
{
    using base = TaskBase<T>;
public:
    explicit Task(const SharedCoroutineHandle<typename base::promise_type>& handle) noexcept : base(handle) {}

    Task(Task&& task) noexcept : base(std::move(task)) {}

    Task(Task&) = delete;

    Task& operator=(Task&) = delete;

    ~Task() = default;

    /**
     * @brief Waits task complete, then return the coroutine result.
     * @return
     */
    T& get_result()
    {
        return (*base::shared_handle_).promise().get_result();
    }

    /**
     * @brief Adds callback to invoke when task completed.
     * @param func
     * @return
     */
    Task& then(std::function<void(T)>&& func)
    {
        (*base::shared_handle_).promise().on_completed(std::move(func));
        return *this;
    }
    /**
     * @brief Adds callback to invoke when task completed.
     * @param func
     * @return
     */
    Task& then(std::function<void()>&& func)
    {
        (*base::shared_handle_).promise().on_completed([func](auto v) {
            func();
        });
        return *this;
    }
};

/**
 * @brief A task represents an operation that produces a result both lazily and asynchronously.
 * When you call a coroutine that returns a task, the coroutine will returns exeuction to the caller when it suspend or return.
 * @note The coroutine handle will only be destroyed when execution completed and relavent task destruct.
 * So it's safe to invoke callbacks after task destructed.
 */
template<>
class Task<void> : public TaskBase<void>
{
    using base = TaskBase<void>;
public:
    explicit Task(const SharedCoroutineHandle<promise_type>& handle) noexcept : base(handle) {}

    Task(Task&& task) noexcept : base(std::move(task)) {}

    Task(Task&) = delete;

    Task& operator=(Task&) = delete;

    ~Task() = default;
    /**
    * @brief Waits task complete.
    */
    void wait()
    {
        (*shared_handle_).promise().wait();
    }
    /**
     * @brief Adds callback to invoke when task completed.
     * @param func
     * @return
     */
    Task& then(std::function<void()>&& func)
    {
        (*shared_handle_).promise().on_completed(std::move(func));
        return *this;
    }

    promise_type& get_promise()
    {
        return (*shared_handle_).promise();
    }
};

inline Task<> details::TaskPromise<void>::get_return_object()
{
    return Task{shared_handle_};
}

}// namespace atlas