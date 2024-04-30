// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "engine.hpp"
#include "tickable/tickable_object.hpp"

namespace atlas
{

Engine* g_engine = nullptr;

void Engine::Startup()
{
    current_time_ = duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());

    tick_task_manager_ = std::make_unique<TickTaskManager>();

    plugin_manager_ = std::make_unique<PluginManager>();
    plugin_manager_->Initialize();
}

void Engine::Shutdown()
{
    plugin_manager_->DeInitialize();
    ModuleManager::Shutdown();
}

void Engine::Loop()
{

}

void Engine::UpdateTickTime()
{
    last_time_ = current_time_;
    current_time_ = duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
    delta_time_ = static_cast<double>((current_time_ - last_time_).count()) * std::nano::num / std::nano::den;
}

} // namespace atlas