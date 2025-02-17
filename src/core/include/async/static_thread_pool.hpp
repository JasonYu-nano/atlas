// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <functional>
#include <queue>

#include "async/thread.hpp"
#include "container/array.hpp"
#include "core_def.hpp"
#include "platform/platform_fwd.hpp"

namespace atlas
{

struct CORE_API ThreadPoolPolicy
{
#if defined __cpp_lib_move_only_function
    using task_type = std::move_only_function<void()>;
#else
    using task_type = std::function<void()>;
#endif
};

template<uint32 NumOfQueues, typename Policy = ThreadPoolPolicy> requires(std::invocable<typename Policy::task_type>)
class StaticThreadPool
{
public:
    using task_type = typename Policy::task_type;

    StaticThreadPool() = default;

    StaticThreadPool(size_t size, StringView work_thread_name = "")
    {
        threads_.reserve(size);
        for (uint32 i = 0; i < size; ++i)
        {
            create_thread(work_thread_name);
        }
    }

    StaticThreadPool(StaticThreadPool&& rhs) noexcept
        : stop_source_(std::move(rhs.stop_source_))
        , mutex_(std::move(rhs.mutex_))
        , awake_signal_(std::move(rhs.awake_signal_))
        , threads_(std::move(rhs.threads_))
        , priority_queue_(std::move(rhs.priority_queue_))
    {}

    StaticThreadPool& operator= (StaticThreadPool&& rhs) noexcept
    {
        join();

        stop_source_ = std::move(rhs.stop_source_);
        std::mutex mutex_ = std::move(rhs.mutex_);
        std::condition_variable new_request_ = std::move(rhs.awake_signal_);
        Array<std::thread> threads_ = std::move(rhs.threads_);
        std::queue<task_type> priority_queue_[NumOfQueues] = std::move(rhs.priority_queue_);

        return *this;
    }

    // Non copyable
    StaticThreadPool(const StaticThreadPool& rhs) = delete;
    StaticThreadPool& operator=(const StaticThreadPool& rhs) = delete;

    ~StaticThreadPool()
    {
        join();
    }

    void push_task(uint32 queue_index, const task_type& task)
    {
        emplace_task(queue_index, task);
    }

    void push_task(uint32 queue_index, task_type&& task)
    {
        emplace_task(queue_index, std::move(task));
    }

    void push_task(const task_type& task) requires (NumOfQueues == 1)
    {
        emplace_task(0, task);
    }

    void push_task(task_type&& task) requires (NumOfQueues == 1)
    {
        emplace_task(0, std::move(task));
    }

    template<typename... Args>
    void emplace_task(uint32 queue_index, Args&&... args)
    {
        ASSERT(queue_index< NumOfQueues && !threads_.is_empty());
        {
            std::lock_guard lock(mutex_);
            priority_queue_[queue_index].emplace(std::forward<Args>(args)...);
        }
        awake_signal_.notify_one();
    }

    bool request_stop()
    {
        if (!stop_source_.stop_possible())
        {
            return false;
        }

        {
            std::lock_guard lock(mutex_);
            stop_source_.request_stop();
        }
        awake_signal_.notify_all(); // wake up all work thread
        return true;
    }

    void join()
    {
        request_stop();

        for (auto&& thread : threads_)
        {
            thread.join();
        }
    }

private:
    void create_thread(StringView work_thread_name)
    {
        static const char* default_name = "static thread pool worker";
        if (work_thread_name.empty())
        {
            work_thread_name = default_name;
        }

        size_t current = threads_.size();
        String thread_name = String::format("{}-{}", work_thread_name, current);

        threads_.emplace([this, thread_name](StopToken stoken) {
            while (true)
            {
                std::unique_lock lock(mutex_);
                if (stoken.stop_requested())
                {
                    break;
                }

                std::optional<task_type> task = pop_task();
                if (!task)
                {
                    awake_signal_.wait(lock);
                    if (stoken.stop_requested())
                    {
                        break;
                    }
                    task = pop_task();
                }

                if (task)
                {
                    lock.unlock();
                    std::invoke(std::move(*task));
                }
            }
            LOG_INFO(core, "{} terminated", thread_name)
        }, stop_source_.get_token());

        PlatformTraits::set_thread_name(threads_.last().native_handle(), thread_name);
    }

    std::optional<task_type> pop_task()
    {
        std::optional<task_type> ret;
        for (uint32 i = 0; i < NumOfQueues; ++i)
        {
            if (!priority_queue_[i].empty())
            {
                ret = std::move(priority_queue_[i].front());
                priority_queue_[i].pop();
                break;
            }
        }

        return ret;
    }

    StopSource stop_source_;
    std::mutex mutex_;
    std::condition_variable awake_signal_;
    Array<std::thread> threads_;
    std::queue<task_type> priority_queue_[NumOfQueues];
};

}// namespace atlas
