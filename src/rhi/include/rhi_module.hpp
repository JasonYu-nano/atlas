// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "module/module_interface.hpp"
#include "tickable/tickable_object.hpp"


namespace atlas
{

class RHI_API RHIModule : public IModule, public TickableObject
{
public:
    void Startup() override;

    void Shutdown() override;

    void Tick(float delta_time) override;

private:
    class WindowsGLContext* ctx_{ nullptr   };
};

} // namespace atlas