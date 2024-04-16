// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "core_def.hpp"

namespace atlas
{

// Use IMPLEMENT_MODULE macro to implement unified creator function.
#define IMPLEMENT_MODULE(module_class, module_name) \
extern "C" DLL_EXPORT IModule* CreateModule() { return new module_class(); }

/**
 * @brief Interfaces for all modules managed by the ModuleManager.
 */
class CORE_API IModule
{
public:
    virtual ~IModule() = default;
    /**
     * @brief Called after construct the module instance.
     * Loads dependent modules here, and they will be guaranteed to be available during Shutdown.
     */
    virtual void Startup() = 0;
    /**
     * @brief Called before the module is unloaded, right before the module object is destroyed.
     */
    virtual void Shutdown() = 0;
};

/**
 * @brief The module interface that needs to be manually called tick by the engine.
 */
class CORE_API IManualTickableModule : public IModule
{
public:
    /**
     * @brief 
     * @param delta_time 
     */
    virtual void Tick(float delta_time) = 0;
};

} // namespace atlas