// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "file_system/path.hpp"
#include "string/string_name.hpp"
#include "misc/delegate.hpp"

#include <filesystem>

namespace atlas
{

class CORE_API Directory
{
public:
    /**
     * @brief Gets root directory of engine.
     * @return
     */
    static Path GetEngineDirectory();
    /**
     * @brief Gets directory of module library.
     * @return
     */
    static Path GetModuleDirectory();
    static Path GetModuleDirectory(const Path& search_path);
    /**
     * @brief Gets directory of engine plugins.
     * @return
     */
    static Path GetEnginePluginsDirectory();
};

}
