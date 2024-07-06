// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <fstream>

#include "project.hpp"
#include "utility/json.hpp"
#include "module/module_manager.hpp"


namespace atlas
{

Project Project::parse(const Path& project_file)
{
    Project proj;
    std::ifstream f(project_file.to_os_path());
    json json_object = json::parse(f);
    if (!json_object.contains("name"))
    {
        return proj;
    }

    auto jo_name = json_object["name"];
    auto name = jo_name.get<std::string>();

    proj.name_ = StringName(StringView(name));

    ModuleManager::add_module_search_path(proj.name_, project_file.parent_path());
    proj.module_ = ModuleManager::load(proj.name_);

    return proj;
}

}// namespace atlas