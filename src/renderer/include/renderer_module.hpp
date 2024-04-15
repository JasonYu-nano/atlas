// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "module/module_interface.hpp"
#include "log/logger.hpp"

namespace atlas
{

class RENDERER_API RendererModule : public IModule
{
public:
    void Startup() final
    {
        LOG_INFO(temp, "renderer module startup.")
    }

    void Shutdown() final
    {
        LOG_INFO(temp, "renderer module shutdown.")
    }
};

} // namespace atlas
