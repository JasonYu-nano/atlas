// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "configuration/config_variable.hpp"
#include "container/map.hpp"
#include "file_system/directory.hpp"
#include "log/logger.hpp"
#include "string/string.hpp"
#include "utility/toml.hpp"

namespace atlas
{

class CORE_API ConfigManager
{
public:
    static ConfigManager& get()
    {
        static ConfigManager cm;
        return cm;
    }

    ~ConfigManager()
    {
        for (auto&& sub_map : category_to_config_variable_map_ | std::views::values)
        {
            for (auto&& var : sub_map | std::views::values)
            {
                delete var;
            }
        }
        category_to_config_variable_map_.clear();
    }
    /**
     * @brief Register a configuration variable. The configuration value will be stored in the toml.
     * @note Variable names cannot be duplicated within the same category.
     * @tparam T
     * @param category 
     * @param variable_name 
     * @param default_value 
     * @return 
     */
    template<typename T>
    IConfigVariable* register_variable(const String& category, const String& variable_name, const T& default_value) requires (is_config_value_type_v<T>)
    {
        toml::table* tb;
        IConfigVariable* var = nullptr;
        if (can_register_variable(category, variable_name, &tb))
        {
            var = new ConfigVariable<T>(variable_name, default_value);
            toml::node* node = tb->get(variable_name);
            if (node && ConfigVariable<T>::is_equivalent(*node))
            {
                // If we already get a valid node, we can simply deserialize from it.
                var->deserialize(*tb);
            }
            else
            {
                var->serialize(*tb);
            }

            auto variable_map_it = category_to_config_variable_map_.find(category);
            if (variable_map_it == category_to_config_variable_map_.end())
            {
                variable_map_it = category_to_config_variable_map_.insert(category, Map<String, IConfigVariable*>());
            }
            variable_map_it->second.insert(variable_name, var);
        }

        return var;
    }
    /**
     * @brief Register a configuration variable. The configuration value will be stored in the toml.
     * Any changes to the configuration value will be copied to the reference value.
     * @note Variable names cannot be duplicated within the same category.
     * @tparam T
     * @param category
     * @param variable_name
     * @param default_value
     * @param ref
     * @return
     */
    template<typename T>
    IConfigVariable* register_variable_ref(const String& category, const String& variable_name, const T& default_value, T& ref) requires (is_config_value_type_v<T>)
    {
        toml::table* tb;
        IConfigVariable* var = nullptr;
        if (can_register_variable(category, variable_name, &tb))
        {
            var = new ConfigVariableRef<T>(variable_name, default_value, ref);
            toml::node* node = tb->get(variable_name);
            if (node && ConfigVariable<T>::is_equivalent(*node))
            {
                // If we already get a valid node, we can simply deserialize from it.
                var->deserialize(*tb);
            }
            else
            {
                var->serialize(*tb);
            }

            auto variable_map_it = category_to_config_variable_map_.find(category);
            if (variable_map_it == category_to_config_variable_map_.end())
            {
                variable_map_it = category_to_config_variable_map_.insert(category, Map<String, IConfigVariable*>());
            }
            variable_map_it->second.insert(variable_name, var);
        }

        return var;
    }
    /**
     * @brief Initialize the configuration values from the toml file.
     * This is usually called at the beginning of engin startup.
     */
    void initialize_variable();
    /**
     * @brief Write the configuration value to the toml file.
     */
    void flush_config() const;

    static Path get_config_save_directory()
    {
        return Directory::get_engine_save_directory() / "config" / separator;
    }
    /**
     * @brief Merge two configuration tables.
     * For values of same path and type, those from the target will be used.
     * If the types are different, the source will be kept.
     * If a path does not exist in the source, the value from the target will be used.
     * @param source
     * @param target
     */
    static void merge_table(toml::table& source, toml::table& target);

private:
    ConfigManager()
    {
        initialize_variable();
    }

    bool can_register_variable(const String& category, const String& variable_name, toml::table** out_table);

    static inline const char* config_file_name_ = "engine_config.toml";

    toml::table config_table_;
    Map<String, Map<String, IConfigVariable*>> category_to_config_variable_map_;
};

/**
 * @brief A helper struct for auto-registering configuration variables.This is useful for registering global configuration variables.
 * @note Configuration variables won't be unregister when ConfigVariableRefRegister destructed.
 * @tparam T
 */
template<typename T>
struct ConfigVariableRefRegister
{
    ConfigVariableRefRegister(const String& category, const String& variable_name, T& ref)
    {
        ConfigManager::get().register_variable_ref(category, variable_name, ref, ref);
    }
};

}// namespace atlas