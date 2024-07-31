// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <type_traits>

#include "container/array.hpp"
#include "core_def.hpp"
#include "core_log.hpp"
#include "core_macro.hpp"
#include "string/string.hpp"
#include "utility/toml.hpp"

namespace atlas
{

namespace details
{

template<typename T>
void deserialize_toml_node(const toml::node& node, T& value) requires (std::is_integral_v<T>)
{
    if (node.is_integer())
    {
        value = **node.as_integer();
    }
}

template<typename T>
void deserialize_toml_node(const toml::node& node, T& value) requires (std::is_floating_point_v<T>)
{
    if (node.is_floating_point())
    {
        value = **node.as_floating_point();
    }
}

inline void deserialize_toml_node(const toml::node& node, bool& value)
{
    if (node.is_boolean())
    {
        value = **node.as_boolean();
    }
}

inline void deserialize_toml_node(const toml::node& node, String& value)
{
    if (node.is_string())
    {
        value = String::from(**node.as_string());
    }
}

template<typename T>
void deserialize_toml_array(const toml::array& array, T& value)
{
    value.resize(array.size());
    size_t idx = 0;
    for (auto&& node : array)
    {
        deserialize_toml_node(node, value[idx]);
        ++idx;
    }
}

template<typename T>
bool is_node_type_equivalent(toml::node_type ntype) requires (std::is_integral_v<T>)
{
    return ntype == toml::node_type::integer;
}

template<typename T>
bool is_node_type_equivalent(toml::node_type ntype) requires (std::is_floating_point_v<T>)
{
    return ntype == toml::node_type::floating_point;
}

template<typename T>
bool is_node_type_equivalent(toml::node_type ntype) requires (std::is_same_v<T, bool>)
{
    return ntype == toml::node_type::boolean;
}

template<typename T>
bool is_node_type_equivalent(toml::node_type ntype) requires (std::is_same_v<T, String>)
{
    return ntype == toml::node_type::string;
}

template<typename T>
bool is_array_type_equivalent(const toml::array& array)
{
    toml::node_type ntype = toml::get_homogeneous_type(array);
    return ntype == toml::node_type::none || is_node_type_equivalent<T>(ntype);
}

}// namespace details

enum class ECVFlag : uint8
{
    // Variable will be store in configuration file and load after module loaded.
    StoreInConfig   = 1 << 0,
    // Variable can be overriden in configuration file. Only works with StoreInConfig.
    Overriden       = 1 << 1,
    // Variable can be modify by console command.
    Mutable         = 1 << 2,
};

ENUM_BIT_MASK(ECVFlag);

template <typename T>
inline constexpr bool is_cv_underlying_type_v = std::is_arithmetic_v<T> || std::is_same_v<T, bool> || std::is_same_v<T, String>;

template<typename T>
struct IsConfigVariableType
{
    static constexpr bool value = is_cv_underlying_type_v<T>;
};

template<typename T>
struct IsConfigVariableType<Array<T>>
{
    static constexpr bool value = is_cv_underlying_type_v<T>;
};

template <typename T>
inline constexpr bool is_config_value_type_v = IsConfigVariableType<T>::value;

class IConfigVariable
{
    friend class ConfigManager;
public:
    virtual ~IConfigVariable() = default;
    /**
     * @brief To indicate whether a node is equivalent to the variable. This usually indicates that both are of the same type.
     * @param node
     * @return
     */
    static bool is_equivalent(const toml::node& node) VIRTUAL_IMPL(core, return false;)
protected :
        /**
     * @brief Serialize self to the table.
     */
    virtual void serialize(toml::table&) = 0;
    /**
     * @brief Deserialize from the table.
     */
    virtual void deserialize(const toml::table&) = 0;
};

/**
 * @brief The generic implementation of the interface IConfigVariable.
 * @tparam T
 */
template<typename T> requires (is_config_value_type_v<T>)
class ConfigVariable : public IConfigVariable
{
public:
    using value_type = T;
    using const_reference = const T&;

    ConfigVariable(const String& name, const_reference default_value)
        : name_(name)
        , value_(default_value)
    {}

    const_reference value() const
    {
        return value_;
    }

    static bool is_equivalent(const toml::node& node)
    {
        if constexpr (is_cv_underlying_type_v<value_type>)
        {
            return details::is_node_type_equivalent<value_type>(node.type());
        }
        else
        {
            if (node.is_array())
            {
                return details::is_array_type_equivalent<typename value_type::value_type>(*node.as_array());
            }
        }
        return false;
    }
protected:

    void serialize(toml::table& tb) override
    {
        if constexpr (is_cv_underlying_type_v<value_type>)
        {
            tb.insert(name_, value_);
        }
        else
        {
            toml::array a;
            a.insert(a.begin(), value_.begin(), value_.end());
            tb.insert(name_, a);
        }
    }

    void deserialize(const toml::table& tb) override
    {
        if (const toml::node* node = tb.get(name_))
        {
            if constexpr (is_cv_underlying_type_v<value_type>)
            {
                details::deserialize_toml_node<value_type>(*node, value_);
                on_value_changed();
            }
            else
            {
                if (node->is_array())
                {
                    details::deserialize_toml_array<value_type>(*node->as_array(), value_);
                    on_value_changed();
                }
            }
        }
    }

    virtual void on_value_changed() {};

    String name_;
    value_type value_;
};

/**
 * @brief It is convenient for users to bind existing variables to configuration variables.
 * @tparam T
 */
template<typename T>
class ConfigVariableRef : public ConfigVariable<T>
{
    using base = ConfigVariable<T>;
public:
    using value_type = typename base::value_type;
    using reference = T&;
    using const_reference = typename base::const_reference;

    ConfigVariableRef(const String& name, const_reference default_value, reference ref)
        : base(name, default_value)
        , ref_(ref)
    {}

protected:
    void on_value_changed() override
    {
        ref_ = base::value_;
    }

    reference ref_;
};

}// namespace atlas