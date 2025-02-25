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

    String project_name = project_file.normalize().to_string();
    project_name.remove(project_suffix_);

    size_t index = project_name.last_index_of(Path::preferred_separator_);
    if (index != INDEX_NONE)
    {
        project_name.remove(0, index + 1);
        proj.name_ = project_name;

        ModuleManager::add_module_search_path(proj.name_, project_file.parent_path());
        proj.module_ = ModuleManager::load(proj.name_);
    }

    return proj;
}

}// namespace atlas