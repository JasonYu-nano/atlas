// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "module/module_interface.hpp"
#include "log/logger.hpp"
#include "opengl/draw_triangle.hpp"

namespace atlas
{

DEFINE_LOGGER(project);

class EMPTY_PROJECT_API EmptyProjectModule : public IModule
{
public:
    void startup() override
    {
        LOG_INFO(project, "project module startup")
        draw_triangle_.initialize();
    }

    void shutdown() override
    {
        draw_triangle_.deinitialize();
        LOG_INFO(project, "project module Shutdown")
    }

private:
    DrawTriangle draw_triangle_;
};

}// namespace atlas
