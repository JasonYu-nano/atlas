// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <fstream>

#include "configuration/config_manager.hpp"
#include "platform/platform_fwd.hpp"

using namespace toml;

namespace atlas
{

static void assign_node(node& source, node& target)
{
    using namespace toml;
    switch (source.type())
    {
        case node_type::boolean:
        {
            *source.as_boolean() = **target.as_boolean();
            break;
        }
        case node_type::string:
        {
            *source.as_string() = **target.as_string();
            break;
        }
        case node_type::integer:
        {
            *source.as_integer() = **target.as_integer();
            break;
        }
        case node_type::floating_point:
        {
            *source.as_floating_point() = **target.as_floating_point();
            break;
        }
        case node_type::date:
        {
            *source.as_date() = **target.as_date();
            break;
        }
        case node_type::time:
        {
            *source.as_time() = **target.as_time();
            break;
        }
        case node_type::date_time:
        {
            *source.as_date_time() = **target.as_date_time();
            break;
        }
        default:
        {

        }
    }
}

static void assign_node(array& source, array& target)
{
    node_type tar_ntype = get_homogeneous_type(target);
    if (tar_ntype == node_type::none)
    {
        source.clear();
        return;
    }

    node_type src_ntype = get_homogeneous_type(source);
    if (src_ntype == node_type::none)
    {
        source = target;
        return;
    }

    if (src_ntype != tar_ntype)
    {
        return;
    }

    source = target;
}

void ConfigManager::initialize_variable()
{
    Path config_file = get_config_save_directory() / config_file_name_;
    if (!config_file.exists())
    {
        return;
    }

    parse_result result = parse_file(config_file.to_string());
    merge_table(config_table_, result);

    // Try to load platform-specific configuration.
    // The platform-specific configuration will override common configuration.
    Path platform_config_file = get_config_save_directory() / PlatformTraits::alias_name_ / config_file_name_;
    if (platform_config_file.exists())
    {
        result = parse_file(platform_config_file.to_string());
        merge_table(config_table_, result);
    }

    for (auto&& variable_map : category_to_config_variable_map_)
    {
        node* node = config_table_.get(variable_map.first);
        if (node && node->is_table())
        {
            for (auto&& variable : variable_map.second)
            {
                variable.second->deserialize(*node->as_table());
            }
        }
    }
}

void ConfigManager::flush_config() const
{
    if (config_table_.empty())
    {
        return;
    }

    Path save_dir = get_config_save_directory();
    if (!save_dir.exists())
    {
        Directory::make_dir_tree(save_dir);
        ASSERT(save_dir.exists());
    }

    Path config_file = save_dir / config_file_name_;

    std::ofstream file(config_file.to_std_path());
    if (file.is_open())
    {
        file << config_table_;
        file.close();
    }
}

void ConfigManager::merge_table(table& source, table& target)
{
    if (&source == &target)
    {
        return;
    }

    for (auto&& pair : target)
    {
        if (!source.contains(pair.first))
        {
            // Insert a key to source that does not exist.
            source.insert(pair.first, pair.second);
            continue;
        }

        auto&& node = source.at(pair.first);
        if (node.type() != pair.second.type())
        {
            // Configuration type has changed, so we discarded the old version.
            continue;
        }

        switch (node.type())
        {
            case node_type::table: {
                merge_table(*node.as_table(), *pair.second.as_table());
                break;
            }
            case node_type::array: {
                assign_node(*node.as_array(), *pair.second.as_array());
                break;
            }
            default: {
                assign_node(node, pair.second);
            }
        }
    }
}
bool ConfigManager::can_register_variable(const String& category, const String& variable_name, table** out_table)
{
    auto variable_map_it = category_to_config_variable_map_.find(category);
    if (variable_map_it != category_to_config_variable_map_.end())
    {
        if (variable_map_it->second.contains(variable_name))
        {
            LOG_WARN(core, "Duplicate variable name {} is not allowd", variable_name);
            return false;
        }
    }

    node* n = config_table_.get(category);
    if (n == nullptr)
    {
        auto v = config_table_.insert(category, toml::table());
        if (v.second)
        {
            n = config_table_.get(category);
            ASSERT(n);
        }
    } else if (!n->is_table())
    {
        LOG_WARN(core, "Category {} is conflicts with an existing node", category);
        n = nullptr;
    }

    if (n)
    {
        *out_table = n->as_table();
    }
    return !!n;
}

}// namespace atlas