// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "file_system/path.hpp"
#include "string/string_name.hpp"

namespace atlas
{

class Project
{
public:
    static Project parse(const Path& project_file);

    bool is_valid() const
    {
        return !!module_;
    }
private:
    StringName name_;
    class IModule* module_{ nullptr };
};

}// namespace atlas
