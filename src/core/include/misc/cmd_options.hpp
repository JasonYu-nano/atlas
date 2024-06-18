// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "core_log.hpp"
#include "string/string.hpp"
#include "container/array.hpp"

namespace atlas
{

namespace details
{

enum class ECommandOptType
{
    None,
    Boolean,
    Integeral,
    FloatPoint,
    String,
};


template<typename T>
struct CommandOptTraits
{
    static constexpr auto type = ECommandOptType::None;
    using backend_type = T;
};

template<>
struct CommandOptTraits<bool>
{
    static constexpr auto type = ECommandOptType::Boolean;
    using backend_type = bool;
};

template<>
struct CommandOptTraits<String>
{
    static constexpr auto type = ECommandOptType::String;
    using backend_type = String;
};

struct FloatPointOptTraits
{
    static constexpr auto type = ECommandOptType::FloatPoint;
    using backend_type = double;
};

template<>
struct CommandOptTraits<double> : FloatPointOptTraits {};

template<>
struct CommandOptTraits<float> : FloatPointOptTraits {};

struct IntegralOptTraits
{
    static constexpr auto type = ECommandOptType::Integeral;
    using backend_type = int64;
};

template<>
struct CommandOptTraits<int64> : IntegralOptTraits {};

template<>
struct CommandOptTraits<int32> : IntegralOptTraits {};

template<>
struct CommandOptTraits<int16> : IntegralOptTraits {};

template<>
struct CommandOptTraits<int8> : IntegralOptTraits {};


template<typename T>
bool command_opt_setter(T&)
{
    return false;
};

template<typename T>
bool command_opt_setter(T&, StringView)
{
    return false;
};

// boolean type
template<>
inline bool command_opt_setter<bool>(bool& src)
{
    src = true;
    return true;
};

template<>
inline bool command_opt_setter<bool>(bool& src, StringView value)
{
    bool is_set = false;
    if (value == "true")
    {
        src = true;
        is_set = true;
    }
    else if (value == "false")
    {
        src = false;
        is_set = true;
    }
    return is_set;
};

// integral type
template<>
inline bool command_opt_setter<int64>(int64& src, StringView value)
{
    src = std::strtoll(value.data(), nullptr, 10);
    return true;
};

// float point type
template<>
inline bool command_opt_setter<double>(double& src, StringView value)
{
    src = std::strtod(value.data(), nullptr);
    return true;
};

// string type
template<>
inline bool command_opt_setter<String>(String& src, StringView value)
{
    src = value;
    return true;
};

}// namespace details

using details::ECommandOptType;
using details::CommandOptTraits;
using details::command_opt_setter;

template<typename T>
class CommandOptionImpl;

class CommandOption
{
    friend class CommandParser;
public:
    explicit CommandOption(ECommandOptType type, StringView name, StringView short_name, StringView description)
        : type_(type)
        , name_(name.data(), name.length())
        , short_name_(short_name.data(), short_name.length())
        , description_(description.data(), description.length())
    {}

    virtual ~CommandOption() = default;

    template<typename T>
    std::optional<T> value() const
    {
        if (CommandOptTraits<T>::type == type_ && has_value())
        {
            if (const void* pointer = get_raw_value())
            {
                auto val = reinterpret_cast<const typename CommandOptTraits<T>::backend_type*>(pointer);
                return std::optional<T>(static_cast<const T>(*val));
            }
        }
        return {};
    }

protected:
    virtual void set() {};
    virtual void set(StringView view) = 0;
    virtual const void* get_raw_value() const = 0;
    virtual bool has_value() const = 0;

    ECommandOptType type_{ ECommandOptType::None };
    String name_;
    String short_name_;
    String description_;
};

template<typename T>
class CommandOptionImpl final : public CommandOption
{
    using base = CommandOption;
public:
    explicit CommandOptionImpl(StringView name, StringView short_name, StringView description)
        : base(CommandOptTraits<T>::type, name, short_name, description) {}

    CommandOptionImpl(StringView name, StringView short_name, StringView description, T default_value)
        : base(CommandOptTraits<T>::type, name, short_name, description), value_(default_value), has_value_(true) {}

protected:
    void set() override
    {
        has_value_ |= command_opt_setter<T>(value_);
    };

    void set(StringView view) override
    {
        has_value_ |= command_opt_setter<T>(value_, view);
    }

    bool has_value() const override
    {
        return has_value_;
    }

    const void* get_raw_value() const override
    {
        return &value_;
    }

    T value_{ T{} };
    bool has_value_{ false };
};

/**
 * @brief Static command line arguments parser.
 *
 * Starts by defining the command options, and then call ParseCommandLineOptions.
 * If you wants to define option before execute main function, @see DEFINE_COMMAND_OPTION.
 */
class CORE_API CommandParser
{
    struct OptionList
    {
        Array<CommandOption*> list;

        ~OptionList()
        {
            std::ranges::for_each(list, [](auto&& opt) { delete opt; });
        }
    };

public:
    /**
     * @brief Use the added options to resolve the given argv.
     * @param argc
     * @param argv
     */
    static void parse_command_line_options(int32 argc, char** argv)
    {
        parse_command_line_internal(argc, argv);
    }
    /**
     * @brief Add a command option.
     * @note The option name and short name can not be duplicated.
     * @tparam Type Supports boolean, string, integral, float point.
     * @param name Option name.
     * @param short_name Option short name.
     * @param description Descrition of the option.
     * @return
     */
    template<typename Type>
    static CommandOption* add_option(StringView name, StringView short_name, StringView description)
    {
        static_assert(CommandOptTraits<Type>::type != ECommandOptType::None);

        CommandOption* opt = nullptr;

        if (name == "")
        {
            LOG_WARN(core, "empty command option name is not allowed.");
        }
        else
        {
            if (!contains_option_name(name, short_name))
            {
                opt = new CommandOptionImpl<typename CommandOptTraits<Type>::backend_type>(name, short_name, description);
                options_.list.add(opt);
            }
            else
            {
                LOG_WARN(core, "duplicate command options name -{0} -{1}", name, short_name);
            }
        }

        return opt;
    }
    /**
     * @brief Add a command option with default value.
     * @note The option name and short name can not be duplicated.
     * @tparam Type Supports boolean, string, integral, float point.
     * @param name Option name.
     * @param short_name Option short name.
     * @param description Descrition of the option.
     * @param default_value 
     * @return 
     */
    template<typename Type>
    static CommandOption* add_option(StringView name, StringView short_name, StringView description, typename CallTraits<Type>::param_type default_value)
    {
        static_assert(CommandOptTraits<Type>::type != ECommandOptType::None);

        CommandOption* opt = nullptr;

        if (name == "")
        {
            LOG_WARN(core, "empty command option name is not allowed.");
        }
        else
        {
            if (!contains_option_name(name, short_name))
            {
                opt = new CommandOptionImpl<typename CommandOptTraits<Type>::backend_type>(name, short_name, description, default_value);
                options_.list.add(opt);
            }
            else
            {
                LOG_WARN(core, "duplicate command options name -{0} -{1}", name, short_name);
            }
        }

        return opt;
    }
    /**
     * @brief Gets value of given option.
     * @tparam Type
     * @param name
     * @return Optional value of given option.
     */
    template<typename Type>
    static std::optional<Type> value_of(StringView name)
    {
        if (const CommandOption* opt = get_option(name))
        {
            return opt->value<Type>();
        }

        return {};
    }
    /**
     * @brief Checks whether given name is duplicated.
     * @param name
     * @param short_name
     * @return
     */
    static bool contains_option_name(StringView name, StringView short_name)
    {
        const size_t index = options_.list.find([&](auto&& opt) {
            return opt->name_ == name || (short_name != "" && opt->short_name_ == short_name);
        });
        return index != INDEX_NONE;
    }
    /**
     * @brief Gets option instance by given option name.
     * @param name
     * @return
     */
    static CommandOption* get_option(StringView name);

private:
    static void parse_command_line_internal(int32 argc, char** argv);

    static inline OptionList options_;
};

/**
 * @brief Helper struct used to add option before execute main function.
 * @tparam T
 */
template<typename T>
struct CommandOptionRegister
{
    CommandOptionRegister(StringView name, StringView short_name, StringView description)
    {
        CommandParser::add_option<T>(name, short_name, description);
    }

    CommandOptionRegister(StringView name, StringView short_name, StringView description, typename CallTraits<T>::param_type default_value)
    {
        CommandParser::add_option<T>(name, short_name, description, default_value);
    }
};

// Use this macro to add option before execute main function.
#define DEFINE_COMMAND_OPTION(type, name, short_name, description) \
    namespace cmd_opt_private { \
    static CommandOptionRegister<type> _register_##type##_##name(#name, short_name, description); \
    }

#define DEFINE_COMMAND_OPTION_DEFAULT(type, name, short_name, description, default_value) \
    namespace cmd_opt_private { \
    static CommandOptionRegister<type> _register_##type##_##name(#name, short_name, description, default_value); \
    }

}// namespace atlas


