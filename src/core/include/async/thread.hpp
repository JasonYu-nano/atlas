// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <thread>

#include "core_def.hpp"
#include "misc/stop_token.hpp"
#include "platform/windows/windows_platform_traits.hpp"

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

    template <typename Callable, typename... Args, typename = std::enable_if_t<!std::is_same_v<std::remove_cvref_t<Callable>, Thread>>>
    explicit Thread(Callable&& callable, Args&&... args) requires(std::is_invocable_v<std::decay_t<Callable>, std::decay_t<Args>...>)
        : thread_(std::forward<Callable>(callable), std::forward<Args>(args)...)
    {}

    template <typename Callable, typename... Args, typename = std::enable_if_t<!std::is_same_v<std::remove_cvref_t<Callable>, Thread>>>
    explicit Thread(Callable&& callable, Args&&... args) requires(std::is_invocable_v<std::decay_t<Callable>, StopToken, std::decay_t<Args>...>)
        : thread_(std::thread(std::forward<Callable>(callable), stop_source_.get_token(), std::forward<Args>(args)...))
    {}

    Thread(Thread&& rhs) noexcept
        : stop_source_(std::move(rhs.stop_source_))
        , thread_(std::move(rhs.thread_))
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

    void set_name(const String& name)
    {
        PlatformTraits::set_thread_name(native_handle(), name);
    }

    static uint32 hardware_concurrency() noexcept
    {
        return std::thread::hardware_concurrency();
    }
private:
    StopSource stop_source_;
    std::thread thread_;
};

namespace this_thread
{

CORE_API inline void* native_handle()
{
    return PlatformTraits::get_this_thread_handle();
}

CORE_API inline void set_name(const String& name)
{
    PlatformTraits::set_thread_name(PlatformTraits::get_this_thread_handle(), name);
}

}// namespace this_thread

}// namespace atlas
