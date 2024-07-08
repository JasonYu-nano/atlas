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
    virtual void execute(float delta_time) = 0;
    /**
     * Determines whether the tick can be called from an asynchronous thread.
     */
    virtual bool allowed_parallel() = 0;
    /**
     * @brief Determines whether the tick can be called when game paused.
     */
    virtual bool allowed_tick_when_pause() = 0;
protected:
    virtual void register_self() = 0;
    virtual void unregister_self() = 0;
};

}// namespace atlas