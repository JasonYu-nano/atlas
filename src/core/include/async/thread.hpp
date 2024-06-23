// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <thread>

#include "core_def.hpp"
#include "misc/stop_token.hpp"

namespace atlas
{

/**
 * @brief This is a personal implementation of the std::jthread since clang doesn't support jthread yet.
 */
class CORE_API Thread
{
public:
    using id                    = std::thread::id;
    using native_handle_type    = std::thread::native_handle_type;

    Thread() = default;

    template <typename Callable, typename... Args, typename = std::enable_if_t<!std::is_same<std::remove_cvref_t<Callable>, Thread>::value>>
    explicit Thread(Callable&& callable, Args&&... args)
        : thread_(std::forward<Callable>(callable), std::forward<Args>(args)...)
    {}

    Thread(Thread&& rhs) noexcept
        : thread_(std::move(rhs.thread_))
        , stop_source_(std::move(rhs.stop_source_))
    {}

    Thread(const Thread& rhs) = delete;

    ~Thread()
    {
        if (thread_.joinable())
        {
            thread_.join();
        }
    }

    Thread& operator= (Thread&& rhs) noexcept
    {
        Thread{std::move(rhs)}.swap(*this);
        return *this;
    }

    Thread& operator= (const Thread& rhs) = delete;

    NODISCARD bool joinable() const
    {
        return thread_.joinable();
    }

    NODISCARD id get_id() const
    {
        return thread_.get_id();
    }

    NODISCARD native_handle_type native_handle()
    {
        return thread_.native_handle();
    }

    void join()
    {
        thread_.join();
    }

    void detach()
    {
        thread_.detach();
    }

    void swap(Thread& rhs)
    {
        std::swap(stop_source_, rhs.stop_source_);
        std::swap(thread_, rhs.thread_);
    }

    NODISCARD StopSource get_stop_source() noexcept
    {
        return stop_source_;
    }

    NODISCARD StopToken get_stop_token() const noexcept
    {
        return stop_source_.get_token();
    }

    void request_stop()
    {
        stop_source_.request_stop();
    }

    static uint32 hardware_concurrency() noexcept
    {
        return std::thread::hardware_concurrency();
    }
private:
    std::thread thread_;
    StopSource stop_source_;
};

}// namespace atlas
