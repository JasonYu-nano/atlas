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

    const Path& get_directory() const
    {
        return project_dir_;
    }

    static inline const char* project_suffix_ = ".project.json";

private:

    StringName name_;
    Path project_dir_;
    class IModule* module_{ nullptr };
};

}// namespace atlas
