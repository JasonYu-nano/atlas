// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <utility>

namespace atlas
{

template<typename Callable>
class ScopeGuard
{
public:
    ScopeGuard() = default;
    explicit ScopeGuard(const Callable& callable) : callable_(callable) {}
    explicit ScopeGuard(Callable&& callable) noexcept : callable_(callable) {}
    ScopeGuard(const ScopeGuard& rhs) = delete;
    ScopeGuard(ScopeGuard&& rhs) noexcept : callable_(std::move(rhs.callable_)), activated_(std::exchange(rhs.activated_, false)) {}

    ~ScopeGuard()
    {
        if (activated_)
        {
            callable_();
        }
    }

    void Release()
    {
        activated_ = false;
    }
private:
    Callable callable_;
    bool activated_{ true };
};

}// namespace atlas