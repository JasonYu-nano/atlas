// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "toml++/toml.hpp"

#include "plugin_manager.hpp"
#include "file_system/directory.hpp"

namespace atlas
{

void PluginManager::initialize()
{
#if WITH_EDITOR
    scan_plugins(Directory::get_engine_plugins_directory());
#endif
}

void PluginManager::scan_plugins(const Path& directory)
{
    for (auto const& dir_entry : std::filesystem::directory_iterator(directory))
    {
        if (dir_entry.is_directory())
        {
            auto plugin_path = dir_entry.path() / "plugin.toml";
            if (std::filesystem::exists(plugin_path))
            {
                parse_plugin_description(String::from(plugin_path));
            }
        }
    }
}

EModuleType ConvertToModuleType(StringView view)
{
    constexpr StringView runtime = "runtime";
    constexpr StringView editor = "editor";

    if (view == runtime)
    {
        return EModuleType::Runtime;
    }

    if (view == editor)
    {
        return EModuleType::Editor;
    }

    return EModuleType::Runtime;
}

void ParsePluginBaseInfo(PluginDesc& desc, const toml::table& config)
{
    if (auto name_view = config["name"].value<std::string_view>())
    {
        desc.name = *name_view;
    }
}

void ParsePluginModule(PluginDesc& desc, const toml::table& config)
{
    if (auto module_array = config["modules"].as_array())
    {
        for (auto&& module_node : *module_array)
        {
            if (auto table_node = module_node.as_table())
            {
                auto&& name_node = table_node->get("name");
                auto&& type_node = table_node->get("type");

                if (name_node && type_node)
                {
                    PluginModuleDesc module_desc;
                    if (auto module_name = name_node->value<std::string_view>())
                    {
                        module_desc.module_name = *module_name;
                    }

                    if (module_desc.module_name.is_none())
                    {
                        continue;
                    }

                    StringView type_view = type_node->value_or("runtime");
                    module_desc.type = ConvertToModuleType(type_view);

                    desc.modules.add(std::move(module_desc));
                }
            }
        }
    }
}

void ParsePluginDependency(PluginDesc& desc, const toml::table& config)
{
    if (auto dependency_array = config["dependency_plugins"].as_array())
    {
        for (auto&& plugin_node : *dependency_array)
        {
            if (auto name_view = plugin_node.value<std::string_view>())
            {
                desc.dependency_plugins.insert(*name_view);
            }
        }
    }
}

void PluginManager::parse_plugin_description(StringView file_path)
{
    auto config = toml::parse_file(file_path);
    PluginDesc plugin_desc;

    ParsePluginBaseInfo(plugin_desc, config);
    if (plugin_desc.name.is_none())
    {
        // plugin name is necessary
        return;
    }

    ParsePluginModule(plugin_desc, config);
    ParsePluginDependency(plugin_desc, config);

    plugins_.add(std::move(plugin_desc));
}

} // namespace atlas