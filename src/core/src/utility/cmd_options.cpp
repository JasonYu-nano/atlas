// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "utility/cmd_options.hpp"

atlas::CommandOption* atlas::CommandParser::get_option(StringView name)
{
    size_t index = INDEX_NONE;
    if (name != "")
    {
        index = options_.list.find([&](auto&& opt) {
            return opt->name_ == name || opt->short_name_ == name;
        });
    }

    return index == INDEX_NONE ? nullptr : options_.list[index];
}

void atlas::CommandParser::parse_command_line_internal(int32 argc, char** argv)
{
    CommandOption* cmd_arg = nullptr;
    for (int i = 1; i < argc; i++)
    {
        char* arg = argv[i];

        if (arg[0] != '-' || arg[1] == '\0')
        {
            // must be a program argument: "-" is an argument, not a flag
            if (cmd_arg)
            {
                cmd_arg->set(arg);
                cmd_arg = nullptr;
            }
            continue;
        }

        if (cmd_arg)
        {
            // option not follow an argument. set an empty value.
            cmd_arg->set();
            cmd_arg = nullptr;
        }

        const size_t len = strlen(arg);
        size_t equal_index = 0;
        // try matches : -o=arg
        for (size_t j = 1; j < len; ++j)
        {
            if (arg[j] == '=')
            {
                equal_index = j;
                break;
            }
        }

        if (equal_index == 0)
        {
            // no equal sign found. it's a pure options.
            const int32 offset = arg[1] == '-' ? 2 : 1;
            cmd_arg = get_option(StringView(arg + offset, len - offset));
        }
        else
        {
            StringView option(arg, equal_index);
            argv[i] = arg + equal_index + 1;
            --i;

            if (option.length() > 1)
            {
                const int32 offset = option[1] == '-' ? 2 : 1;
                cmd_arg = get_option(StringView(option.data() + offset, option.length() - offset));
            }
        }
    }
}