// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "engine.hpp"

namespace atlas
{

Engine* g_engine = nullptr;

void Engine::Startup()
{
    plugin_manager_ = std::make_unique<PluginManager>();
    plugin_manager_->Initialize();
}

void Engine::Shutdown()
{
    plugin_manager_->DeInitialize();
    ModuleManager::Shutdown();
}

} // namespace atlas