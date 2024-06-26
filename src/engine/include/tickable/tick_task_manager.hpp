// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "container/unordered_set.hpp"
#include "tickable/tick_task.hpp"

namespace atlas
{

class ENGINE_API TickTaskManager
{
public:
    void add_task(TickTask* task)
    {
        if (task)
        {
            task_set_.insert(task);
        }
    }

    void remove_task(TickTask* task)
    {
        if (task)
        {
            task_set_.remove(task);
        }
    }

    void tick(float delta_time)
    {
        for (auto task : task_set_)
        {
            task->execute(delta_time);
        }
    }

private:
    UnorderedSet<TickTask*> task_set_;
};

}// namespace atlas