// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "file_system/path.hpp"
#include "utility/delegate_fwd.hpp"

namespace atlas
{

DECLARE_DELEGATE_RET(GetProjectDirectoryImpl, Path);

class CORE_API Directory
{
public:
    static bool make_dir(const Path& path);
    static bool make_dir_tree(const Path& path);
    /**
     * @brief Gets root directory of engine.
     * @return
     */
    static const Path& get_engine_directory();
    /**
     * @brief Gets directory of engine module library.
     * @return
     */
    static Path get_engine_module_directory();
    /**
     * @brief Format module library directory by given path.
     * @return
     */
    static Path get_module_directory(const Path& search_path);
    /**
     * @brief Gets directory of engine plugins.
     * @return
     */
    static Path get_engine_plugins_directory();
    /**
     * @brief Gets directory of engine save.
     * @return
     */
    static Path get_engine_save_directory();
    /**
     * @brief Gets directory of the current project.
     * @return
     */
    static Path get_project_directory();

    static inline GetProjectDirectoryImpl get_project_directory_impl_;
};

}