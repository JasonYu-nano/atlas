// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "module/module_interface.hpp"
#include "log/logger.hpp"

namespace atlas
{

DEFINE_LOGGER(project);

class EMPTY_PROJECT_API EmptyProjectModule : public IModule
{
    void startup() override
    {
        LOG_INFO(project, "project module startup")
    }
    void shutdown() override
    {
        LOG_INFO(project, "project module Shutdown")
    }
};

}// namespace atlas
