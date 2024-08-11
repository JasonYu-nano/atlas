// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "asset/serialize_example.hpp"
#include "log/logger.hpp"
#include "module/module_interface.hpp"
#include "opengl/draw_triangle.hpp"

namespace atlas
{

DEFINE_LOGGER(project);

class GALLERY_API GalleryModule : public IModule
{
public:
    void startup() override
    {
        LOG_INFO(project, "project module startup")
        draw_triangle_.initialize();
        serialize_example_.initialize();
    }

    void shutdown() override
    {
        draw_triangle_.deinitialize();
        serialize_example_.deinitialize();
        LOG_INFO(project, "project module Shutdown")
    }

private:
    DrawTriangle draw_triangle_;
    SerializeExample serialize_example_;
};

}// namespace atlas
