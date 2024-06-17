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

void Engine::startup(int argc, char** argv)
{
    CommandParser::parse_command_line_options(argc, argv);

    current_time_ = duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());

    tick_task_manager_ = std::make_unique<TickTaskManager>();

    plugin_manager_ = std::make_unique<PluginManager>();
    plugin_manager_->initialize();

    load_project();

    if (!project_.is_valid())
    {

    }
}

void Engine::shutdown()
{
    plugin_manager_->deinitialize();
    ModuleManager::shutdown();
}

void Engine::loop()
{

}

void Engine::update_tick_time()
{
    last_time_ = current_time_;
    current_time_ = duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
    delta_time_ = static_cast<double>((current_time_ - last_time_).count()) * std::nano::num / std::nano::den;
}

void Engine::load_project()
{
    std::optional<String> project_path = CommandParser::value_of<String>("project");
    if (!project_path.has_value())
    {
        return;
    }

    Path path = *project_path;
    path = path.normalize();

    if (path.extension().to_string() != ".aproj")
    {
        return;
    }

    if (path.is_relative())
    {
        // try convert to absolute path
        const Path root = Directory::get_engine_directory();
        path = root / path;
    }

    project_ = Project::parse(path);
}

} // namespace atlas