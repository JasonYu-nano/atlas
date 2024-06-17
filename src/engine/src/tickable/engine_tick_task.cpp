// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "tickable/engine_tick_task.hpp"
#include "engine.hpp"

namespace atlas
{


void EngineTickTask::register_self()
{
    if (g_engine)
    {
        g_engine->get_tick_task_manager()->add_task(this);
    }
}

void EngineTickTask::unregister_self()
{
    if (g_engine)
    {
        g_engine->get_tick_task_manager()->remove_task(this);
    }
}
}// namespace atlas
