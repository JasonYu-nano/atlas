// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <fstream>

#include "project.hpp"
#include "misc/json.hpp"
#include "module/module_manager.hpp"


namespace atlas
{

Project Project::Parse(const Path& project_file)
{
    Project proj;
    std::ifstream f(project_file.ToOSPath());
    json json_object = json::parse(f);
    if (!json_object.contains("name"))
    {
        return proj;
    }

    auto jo_name = json_object["name"];
    auto name = jo_name.get<std::string>();

    proj.name_ = StringName(StringView(name));

    ModuleManager::AddModuleSearchPath(proj.name_, project_file.ParentPath());
    proj.module_ = ModuleManager::Load(proj.name_);

    return proj;
}

}// namespace atlas