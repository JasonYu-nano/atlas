// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "file_system/path.hpp"

namespace atlas
{

class CORE_API Directory
{
public:
    /**
     * @brief Gets root directory of engine.
     * @return
     */
    static Path get_engine_directory();
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
};

}
