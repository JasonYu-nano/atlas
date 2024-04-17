// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "module/module_interface.hpp"


namespace atlas
{

class RHI_API RHIModule : public IModule
{
public:
    void Startup() override {}
    void Shutdown() override {}
};

} // namespace atlas