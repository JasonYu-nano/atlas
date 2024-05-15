// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "module/module_manager.hpp"
#include "plugin_manager.hpp"
#include "project.hpp"
#include "tickable/tick_task_manager.hpp"

namespace atlas
{

// Global access points of engine instance. Validity is not guaranteed.
extern ENGINE_API class Engine* g_engine;

/**
 * @brief Abstract base class of all engine classes, responsible for management of all game module.
 */
class ENGINE_API Engine
{
public:
    Engine() = default;

    virtual ~Engine() = default;

    virtual void Startup(int argc, char** argv);

    virtual void Shutdown();

    virtual void Loop();

    virtual void RequestShutdown()
    {
        is_shutdown_requested_ = true;
    }

    virtual bool IsShutdownRequested() const
    {
        return is_shutdown_requested_;
    }

    virtual void UpdateTickTime();

    double GetDeletaTime() const{ return delta_time_; }

    TickTaskManager* GetTickTaskManager() const { return tick_task_manager_.get(); }

protected:
    void LoadProject();

    bool is_shutdown_requested_{ false };

    std::chrono::nanoseconds last_time_{ 0 };
    std::chrono::nanoseconds current_time_{ 0 };
    double delta_time_{ 0.0 };

    std::unique_ptr<TickTaskManager> tick_task_manager_{ nullptr };
    std::unique_ptr<PluginManager> plugin_manager_{ nullptr };

    Project project_;
};

} // namespace atlas
