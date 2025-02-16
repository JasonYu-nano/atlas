// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "toml++/toml.hpp"

#include "plugin_manager.hpp"
#include "engine_log.hpp"
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
    if (!directory.exists())
    {
        LOG_INFO(engine, "plugin directory {0} is not exists", directory.to_string());
        return;
    }

    for (auto const& dir_entry : std::filesystem::directory_iterator(directory))
    {
        if (dir_entry.is_directory())
        {
            auto plugin_path = dir_entry.path() / "plugin.toml";
            if (std::filesystem::exists(plugin_path))
            {
                parse_plugin_description(dir_entry.path(), String::from(plugin_path));
            }
        }
    }
}

EModuleType convert_to_module_type(StringView view)
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

void parse_plugin_base_info(PluginDesc& desc, const toml::table& config)
{
    if (auto name_view = config["name"].value<std::string_view>())
    {
        desc.name = *name_view;
    }
}

void parse_plugin_module(PluginDesc& desc, const toml::table& config)
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
                    module_desc.type = convert_to_module_type(type_view);

                    desc.modules.add(std::move(module_desc));
                }
            }
        }
    }
}

void parse_plugin_dependency(PluginDesc& desc, const toml::table& config)
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

void PluginManager::parse_plugin_description(const std::filesystem::path& dir, StringView file_path)
{
    auto config = toml::parse_file(file_path);
    PluginDesc plugin_desc;

    parse_plugin_base_info(plugin_desc, config);
    if (plugin_desc.name.is_none())
    {
        // plugin name is necessary
        return;
    }

    parse_plugin_module(plugin_desc, config);
    parse_plugin_dependency(plugin_desc, config);
    plugin_desc.absolute_path = String::from(dir);

    plugins_.add(std::move(plugin_desc));
}

} // namespace atlas