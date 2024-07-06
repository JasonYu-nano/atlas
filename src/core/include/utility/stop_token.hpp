// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <atomic>
#include <utility>

#include "../core_def.hpp"

namespace atlas
{

namespace details
{

struct StopState
{
    std::atomic<uint32> stop_tokens = 1;
    std::atomic<uint32> stop_sources = 2;

    NODISCARD bool stop_requested() const noexcept
    {
        return (stop_sources.load() & uint32{1}) != 0;
    }

    NODISCARD bool stop_possible() const noexcept
    {
        return stop_sources.load() != 0;
    }

    bool request_stop()
    {
        // Attempts to request stop and call callbacks, returns whether request was successful
        if ((stop_sources.fetch_or(uint32{1}) & uint32{1}) != 0)
        {
            // another thread already requested
            return false;
        }

        return true;
    }
};

}// namespace details

class StopToken
{
    using stop_state = details::StopState;
    friend class StopSource;
public:
    StopToken() = default;
    StopToken(const StopToken& rhs) : state_(rhs.state_)
    {
        if (const auto local = state_)
        {
            local->stop_tokens.fetch_add(1, std::memory_order_relaxed);
        }
    }
    StopToken(StopToken&& rhs) noexcept : state_(std::exchange(rhs.state_, nullptr)) {}

    ~StopToken()
    {
        if (const auto local = state_)
        {
            if (local->stop_tokens.fetch_sub(1, std::memory_order_acq_rel) == 1)
            {
                delete local;
            }
        }
    }

    StopToken& operator= (const StopToken& rhs) noexcept
    {
        StopToken(rhs).swap(*this);
        return *this;
    }

    StopToken& operator= (StopToken&& rhs) noexcept
    {
        StopToken(std::move(rhs)).swap(*this);
        return *this;
    }

    NODISCARD bool operator== (const StopToken& rhs) const noexcept
    {
        return state_ == rhs.state_;
    }

    NODISCARD bool stop_requested() const noexcept
    {
        return state_ != nullptr && state_->stop_requested();
    }

    NODISCARD bool stop_possible() const noexcept
    {
        return state_ != nullptr;
    }

    void swap(StopToken& other) noexcept
    {
        std::swap(state_, other.state_);
    }

    friend void swap(StopToken& lhs, StopToken& rhs) noexcept
    {
        std::swap(lhs.state_, rhs.state_);
    }

private:
    explicit StopToken(stop_state* state) noexcept : state_(state) {}

    stop_state* state_{ nullptr };
};

struct NoStopStateT {};
inline constexpr NoStopStateT no_stop_state;

class StopSource
{
    using stop_state = details::StopState;
public:
    StopSource() : state_(new stop_state()) {}
    explicit StopSource(NoStopStateT) noexcept {}
    StopSource(const StopSource& rhs) noexcept : state_(rhs.state_)
    {
        if (const auto local = state_)
        {
            local->stop_sources.fetch_add(2, std::memory_order_relaxed);
        }
    }
    StopSource(StopSource&& rhs) noexcept : state_(std::exchange(rhs.state_, nullptr)) {}

    ~StopSource()
    {
        if (const auto local = state_)
        {
            if ((local->stop_sources.fetch_sub(2, std::memory_order_acq_rel) >> 1) == 1)
            {
                if (local->stop_tokens.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                    delete local;
                }
            }
        }
    }

    StopSource& operator= (const StopSource& rhs) noexcept
    {
        StopSource(rhs).swap(*this);
        return *this;
    }

    StopSource& operator= (StopSource&& rhs) noexcept
    {
        StopSource(std::move(rhs)).swap(*this);
        return *this;
    }

    NODISCARD bool operator== (const StopSource& rhs) const noexcept
    {
        return state_ == rhs.state_;
    }

    NODISCARD StopToken get_token() const noexcept
    {
        return StopToken(state_);
    }

    NODISCARD bool stop_requested() const noexcept
    {
        return state_ != nullptr && state_->stop_requested();
    }

    NODISCARD bool stop_possible() const noexcept
    {
        return state_ != nullptr;
    }

    bool request_stop() noexcept
    {
        return state_ && state_->request_stop();
    }

    void swap(StopSource& other) noexcept
    {
        std::swap(state_, other.state_);
    }

    friend void swap(StopSource& lhs, StopSource& rhs) noexcept
    {
        std::swap(lhs.state_, rhs.state_);
    }

private:
    stop_state* state_{ nullptr };
};

}// namespace atlas