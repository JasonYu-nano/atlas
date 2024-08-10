// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "asset/serialize_sample.hpp"
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
        serialize_sample_.initialize();
    }

    void shutdown() override
    {
        draw_triangle_.deinitialize();
        serialize_sample_.deinitialize();
        LOG_INFO(project, "project module Shutdown")
    }

private:
    DrawTriangle draw_triangle_;
    SerializeSample serialize_sample_;
};

}// namespace atlas
