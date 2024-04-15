// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "core_def.hpp"

namespace atlas
{
/**
 * @brief Abstract class for all tick request.
 */
class ENGINE_API TickTask
{
public:
    TickTask() = default;
    virtual ~TickTask() = default;
    /**
     * @brief Called from task manager.
     * @param delta_time
     */
    virtual void Execute(float delta_time) = 0;

    /**
     * @brief Determines whether the tick can be called from an asynchronous thread.
     */
    bool allowed_parallel_{ false };
    /**
     * @brief Determines whether the tick can be called when game paused.
     */
    bool allowed_tick_when_pause_{ false };
protected:
    virtual void RegisterSelf() = 0;
    virtual void UnregisterSelf() = 0;
};

}// namespace atlas