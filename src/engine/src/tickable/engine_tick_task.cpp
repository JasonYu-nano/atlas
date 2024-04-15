// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "tickable/engine_tick_task.hpp"
#include "engine.hpp"

namespace atlas
{


void EngineTickTask::RegisterSelf()
{
    if (g_engine)
    {
        g_engine->GetTickTaskManager()->AddTask(this);
    }
}

void EngineTickTask::UnregisterSelf()
{
    if (g_engine)
    {
        g_engine->GetTickTaskManager()->RemoveTask(this);
    }
}
}// namespace atlas
