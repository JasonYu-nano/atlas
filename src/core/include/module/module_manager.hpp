// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "module/module_interface.hpp"
#include "string/string_name.hpp"
#include "container/unordered_map.hpp"
#include "file_system/path.hpp"
#include "platform/platform_fwd.hpp"

namespace atlas
{

struct ReleaseModuleHandle
{
    void operator() (void*& handle) const
    {
        if (handle)
        {
            PlatformTraits::FreeDynamicLibrary(handle);
            handle = nullptr;
        }
    }
};

struct ModuleInfo
{
    static inline int32 current_load_order = INDEX_NONE;
    StringName name;
    int32 load_order = INDEX_NONE;
    // make sure module handle is release after module
    std::unique_ptr<void, ReleaseModuleHandle> module_handle = nullptr;
    std::unique_ptr<IModule> module = nullptr;
};

class CORE_API ModuleManager
{
    using fn_create_module = IModule* (*)();
public:
    static bool is_module_loaded(StringName name);
    /**
     * @brief Loads the specified module.
     * @param name
     * @return
     */
    static IModule* load(StringName name);
    /**
     * @brief Loads the specified module, checking to ensure it exists.
     * @param name
     * @return
     */
    static IModule& load_checked(StringName name)
    {
        IModule* module = load(name);
        ASSERT(module);
        return *module;
    }
    /**
     * @brief Unloads the specified module manually.
     * @note Be careful as you may be unloading another module's dependency too early.
     * @param name
     */
    static void unload(StringName name);
    /**
     * @brief Shutdown module manager and unload all modules.
     */
    static void shutdown();

    static void add_module_search_path(StringName module_name, const Path& path)
    {
        module_search_path_map_.insert(module_name, path);
    }

private:
    ModuleManager() = default;

    static IModule* add_module(StringName name);

    static void create_module_impl(ModuleInfo& module_info);

    static inline std::shared_mutex mutex_;
    static inline UnorderedMap<StringName, ModuleInfo> module_info_map_;
    static inline UnorderedMap<StringName, Path> module_search_path_map_;
};

} // namespace atlas
