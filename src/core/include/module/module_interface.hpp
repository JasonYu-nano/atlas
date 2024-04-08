// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "core_def.hpp"

namespace atlas
{

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

} // namespace atlas