// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "module/module_manager.hpp"
#include "container/array.hpp"
#include "file_system/directory.hpp"
#include "log/logger.hpp"

namespace atlas
{

DEFINE_LOGGER(module_manager);

bool ModuleManager::is_module_loaded(StringName name)
{
    std::shared_lock lock(mutex_);
    return module_info_map_.contains(name);
}

IModule* ModuleManager::load(StringName name)
{
    {
        std::shared_lock lock(mutex_);
        auto module_it = module_info_map_.find(name);
        if (module_it != module_info_map_.end())
        {
            return module_it->second.module.get();
        }
    }

    return add_module(name);
}

void ModuleManager::unload(StringName name)
{
    ModuleInfo pending_unload_module;
    {
        std::unique_lock lock(mutex_);
        auto it = module_info_map_.find(name);
        if (it == module_info_map_.end())
        {
            return;
        }

        pending_unload_module = std::move(it->second);
        module_info_map_.remove(it);
    }

    if (pending_unload_module.module)
    {
        pending_unload_module.module->shutdown();
    }
}

void ModuleManager::shutdown()
{
    struct ShutdownModuleInfo
    {
        StringName name;
        int32 load_order;
        IModule* module;

        bool operator> (const ShutdownModuleInfo& rhs) const
        {
            return load_order < rhs.load_order;
        }

        bool operator< (const ShutdownModuleInfo& rhs) const
        {
            return load_order > rhs.load_order;
        }
    };

    Array<ShutdownModuleInfo> pending_shutdown_modules;
    pending_shutdown_modules.reserve(module_info_map_.size());

    for (auto&& it : module_info_map_)
    {
        pending_shutdown_modules.add(ShutdownModuleInfo{it.first, it.second.load_order, it.second.module.get()});
    }
    // ensure first loaded module shutdown at last.
    std::sort(pending_shutdown_modules.begin(), pending_shutdown_modules.end());
    for (auto&& it = pending_shutdown_modules.rbegin(); it != pending_shutdown_modules.rend(); ++it)
    {
        if (it->module)
        {
            it->module->shutdown();
        }
    }
    pending_shutdown_modules.clear();
    module_info_map_.clear();
}

IModule* ModuleManager::add_module(StringName name)
{
    auto&& it = module_info_map_.end();
    {
        std::unique_lock lock(mutex_);
        // double check to avoid construct duplicate module info.
        auto module_it = module_info_map_.find(name);
        if (module_it != module_info_map_.end())
        {
            return module_it->second.module.get();
        }

        it = module_info_map_.insert(name, ModuleInfo());
        // ensure can always get correct instance.
        it->second.name = name;

        create_module_impl(it->second);
    }
    // to avoid deadlock in Startup(), don't put in lock scope.
    if (it->second.module)
    {
        it->second.module->startup();
    }
    // start up may load other dependency modules. current load order should after dependency modules.
    it->second.load_order = ++ModuleInfo::current_load_order;
    return it->second.module.get();
}

void ModuleManager::create_module_impl(ModuleInfo& module_info)
{
    Path search_path;
    if (Path* p = module_search_path_map_.find_value(module_info.name))
    {
        search_path = *p;
    }
    else
    {
        search_path = Directory::get_engine_directory();
        module_search_path_map_.insert(module_info.name, search_path);
    }

    const Path lib_path = PlatformTraits::get_library_path(Directory::get_module_directory(search_path), module_info.name);

    void* handle = PlatformTraits::load_library(lib_path);
    if (handle)
    {
        module_info.module_handle = std::unique_ptr<void, ReleaseModuleHandle>(handle);
        fn_create_module fn_address = (fn_create_module)PlatformTraits::get_exported_symbol(handle, "CreateModule");
        if (fn_address)
        {
            module_info.module = std::unique_ptr<IModule>(fn_address());
            if (module_info.module == nullptr)
            {
                LOG_ERROR(module_manager, "Failed to create module instance");
                module_info.module_handle.reset();
            }
        }
        else
        {
            LOG_ERROR(module_manager, "Can't find CreateModule function in module library. Maybe miss IMPLEMENT_MODULE.");
            module_info.module_handle.reset();
        }
    }
    else
    {
        LOG_ERROR(module_manager, "Failed to load module library from path {0}", search_path.to_string());
    }
}

} // namespace atlas
