// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "engine.hpp"

#include "project.hpp"
#include "misc/cmd_options.hpp"
#include "misc/json.hpp"

#include <fstream>

namespace atlas
{

Engine* g_engine = nullptr;

DEFINE_COMMAND_OPTION(String, project, "p", "The path of the project to start")

void Engine::Startup(int argc, char** argv)
{
    CommandParser::ParseCommandLineOptions(argc, argv);

    current_time_ = duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());

    tick_task_manager_ = std::make_unique<TickTaskManager>();

    plugin_manager_ = std::make_unique<PluginManager>();
    plugin_manager_->Initialize();

    LoadProject();

    if (!project_.IsValid())
    {

    }
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

void Engine::LoadProject()
{
    std::optional<String> project_path = CommandParser::ValueOf<String>("project");
    if (!project_path.has_value())
    {
        return;
    }

    Path path = *project_path;
    path = path.Normalize();

    if (path.Extension().ToString() != ".aproj")
    {
        return;
    }

    if (path.IsRelative())
    {
        // try convert to absolute path
        const Path root = Directory::GetEngineDirectory();
        path = root / path;
    }

    project_ = Project::Parse(path);
}

} // namespace atlas