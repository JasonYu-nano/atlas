// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <utility>

#include "../core_def.hpp"

namespace atlas
{

template<typename Callable>
class ScopedLambda
{
public:
	explicit ScopedLambda(Callable&& func)
		: callable_(std::forward<Callable>(func))
		, is_cancelled_(false)
	{}

	ScopedLambda(const ScopedLambda& other) = delete;

	ScopedLambda(ScopedLambda&& other) noexcept
        : callable_(std::forward<Callable>(other.callable_))
        , is_cancelled_(other.is_cancelled_)
	{
		other.cancel();
	}

	~ScopedLambda()
	{
		if (!is_cancelled_)
		{
			callable_();
		}
	}

	void cancel()
	{
		is_cancelled_ = true;
	}

	void call_now()
	{
		is_cancelled_ = true;
		callable_();
	}

private:

	Callable callable_;
	bool is_cancelled_;

};

template<typename Callable>
NODISCARD auto on_scope_exit(Callable&& func)
{
	return ScopedLambda<Callable>{ std::forward<Callable>(func) };
}

}// namespace atlas
