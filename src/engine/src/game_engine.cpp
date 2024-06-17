// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "game_engine.hpp"

namespace atlas
{

void GameEngine::startup(int argc, char** argv)
{
    base::startup(argc, argv);
    application_module_ = static_cast<IManualTickableModule*>(ModuleManager::load("application"));
}

void GameEngine::shutdown()
{
    base::shutdown();
}

void GameEngine::loop()
{
    // Process system event first.
    update_tick_time();

    application_module_->tick(delta_time_);
    tick_task_manager_->tick(static_cast<float>(delta_time_));
}


} // namespace atlas