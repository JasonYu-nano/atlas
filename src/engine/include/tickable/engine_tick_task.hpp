// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "tickable/tick_task.hpp"
#include "misc/delegate_fwd.hpp"

namespace atlas
{

DECLARE_DELEGATE_ONE_PARAM(OnEngineTick, float, delta_time);

/**
 * @brief Register tick function in engine loop. Tick function will called in runtime and editor.
 */
class ENGINE_API EngineTickTask : public TickTask
{
    using base = TickTask;
public:
    explicit EngineTickTask(const OnEngineTick& delegate) : base(), tick_delegate_(delegate)
    {
        allowed_tick_when_pause_ = true;
        EngineTickTask::RegisterSelf();
    }

    ~EngineTickTask() override
    {
        EngineTickTask::UnregisterSelf();
    }

    void Execute(float delta_time) override
    {
        tick_delegate_.execute_safe(delta_time);
    }

protected:
    void RegisterSelf() override;
    void UnregisterSelf() override;

    OnEngineTick tick_delegate_;
};

}// namespace atlas