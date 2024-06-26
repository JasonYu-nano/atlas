// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "string/string_name.hpp"
#include "file_system/directory.hpp"
#include "container/set.hpp"

namespace atlas
{

enum class EModuleType : uint8
{
    Runtime,
    Editor,
};

struct ENGINE_API PluginModuleDesc
{
    StringName module_name;
    EModuleType type = EModuleType::Runtime;
};

struct ENGINE_API PluginDesc
{
    StringName name;
    Array<PluginModuleDesc> modules;
    Set<StringName> dependency_plugins;
};

class ENGINE_API PluginManager
{
public:
    void initialize();

    void deinitialize()
    {

    }

private:
    void scan_plugins(const Path& directory);

    void parse_plugin_description(StringView file_path);

    Array<PluginDesc> plugins_;
};

} // namespace atlas
