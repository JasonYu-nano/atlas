// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "module/module_manager.hpp"
#include "plugin_manager.hpp"

namespace atlas
{

// Global access points of engine instance. Validity is not guaranteed.
extern ENGINE_API class Engine* g_engine;

/**
 * @brief Abstract base class of all engine classes, responsible for management of all game module.
 */
class ENGINE_API Engine
{
public:
    Engine() = default;

    virtual ~Engine() = default;

    virtual void Startup();

    virtual void Shutdown();

    virtual void Loop()
    {

    }

    virtual void RequestShutdown()
    {
        is_shutdown_requested_ = true;
    }

    virtual bool IsShutdownRequested() const
    {
        return is_shutdown_requested_;
    }

protected:
    bool is_shutdown_requested_{ false };
    std::unique_ptr<PluginManager> plugin_manager_{ nullptr };
};

} // namespace atlas
