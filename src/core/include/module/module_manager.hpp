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
    static bool IsModuleLoaded(StringName name);
    /**
     * @brief Loads the specified module.
     * @param name
     * @return
     */
    static IModule* Load(StringName name);
    /**
     * @brief Loads the specified module, checking to ensure it exists.
     * @param name
     * @return
     */
    static IModule& LoadChecked(StringName name)
    {
        IModule* module = Load(name);
        ASSERT(module);
        return *module;
    }
    /**
     * @brief Unloads the specified module manually.
     * @note Be careful as you may be unloading another module's dependency too early.
     * @param name
     */
    static void Unload(StringName name);
    /**
     * @brief Shutdown module manager and unload all modules.
     */
    static void Shutdown();

    static void AddModuleSearchPath(StringName module_name, const Path& path)
    {
        module_search_path_map_.insert(module_name, path);
    }

private:
    ModuleManager() = default;

    static IModule* AddModule(StringName name);

    static void CreateModuleImp(ModuleInfo& module_info);

    static inline std::shared_mutex mutex_;
    static inline UnorderedMap<StringName, ModuleInfo> module_info_map_;
    static inline UnorderedMap<StringName, Path> module_search_path_map_;
};

} // namespace atlas
