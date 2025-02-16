// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "asset/asset_manager.hpp"
#include "module/module_manager.hpp"
#include "plugin_manager.hpp"
#include "project.hpp"
#include "io/llio.hpp"
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

    virtual void startup(int argc, char** argv);

    virtual void shutdown();

    virtual void loop();

    virtual void request_shutdown()
    {
        is_shutdown_requested_ = true;
    }

    virtual bool is_shutdown_requested() const
    {
        return is_shutdown_requested_;
    }

    virtual void update_tick_time();

    double get_delta_time() const{ return delta_time_; }

    TickTaskManager* get_tick_task_manager() const { return tick_task_manager_.get(); }

    LowLevelIO& get_llio() const { return llio_; }

    AssetManager* get_asset_manager() const { return asset_manager_.get(); }

    const Project& get_project() const { return project_; }

protected:
    void load_project();

    bool is_shutdown_requested_{ false };

    std::chrono::nanoseconds last_time_{ 0 };
    std::chrono::nanoseconds current_time_{ 0 };
    double delta_time_{ 0.0 };

    mutable LowLevelIO llio_;

    std::unique_ptr<TickTaskManager> tick_task_manager_{ nullptr };
    std::unique_ptr<PluginManager> plugin_manager_{ nullptr };
    std::unique_ptr<AssetManager> asset_manager_{ nullptr };

    Project project_;
};

} // namespace atlas
