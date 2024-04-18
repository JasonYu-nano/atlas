// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "game_engine.hpp"

namespace atlas
{

void GameEngine::Startup()
{
    base::Startup();
    application_module_ = static_cast<IManualTickableModule*>(ModuleManager::Load("application"));
}

void GameEngine::Shutdown()
{
    base::Shutdown();
}

void GameEngine::Loop()
{
    // Process system event first.
    UpdateTickTime();

    application_module_->Tick(static_cast<float>(delta_time_));
    tick_task_manager_->Tick(static_cast<float>(delta_time_));
}


} // namespace atlas