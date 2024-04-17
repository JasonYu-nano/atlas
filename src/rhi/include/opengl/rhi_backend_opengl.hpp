// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "rhi_backend.hpp"

namespace atlas
{

class RHI_API RHIBackendOpenGLInitParams : public RHIBackendInitParams
{

};

class RhiBackendOpenGL : public RhiBackend
{
public:
    explicit RhiBackendOpenGL(const RHIBackendOpenGLInitParams& params)
    {

    }
};

}// namespace atlas