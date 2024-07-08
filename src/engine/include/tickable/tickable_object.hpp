// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "tickable/engine_tick_task.hpp"

namespace atlas
{
/**
 * @brief Allows inherited objects to tick in the engine loop.
 */
class ENGINE_API TickableObject
{
public:
    virtual ~TickableObject() = default;
protected:
    TickableObject() : tick_task_(OnEngineTick::create_raw(this, &TickableObject::on_execution)) {}

    void on_execution(float delta_time)
    {
        if (can_tick())
        {
            tick(delta_time);
        }
    }

    virtual void tick(float delta_time) = 0;

    virtual bool can_tick() { return true; }

    EngineTickTask tick_task_;
};

}// namespace atlas