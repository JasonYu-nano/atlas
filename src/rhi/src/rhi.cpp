// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "rhi.hpp"
#include "opengl/rhi_backend_opengl.hpp"


namespace atlas
{

Rhi* Rhi::Create(ERhiBackend backend, RHIBackendInitParams* params)
{
    auto rhi = std::make_unique<Rhi>();
    switch (backend)
    {
        case ERhiBackend::OpneGL:
        {
            rhi->backend_ = std::make_unique<RhiBackendOpenGL>(*static_cast<RHIBackendOpenGLInitParams*>(params));
            break;
        }
    }

    if (rhi->backend_)
    {
        return rhi.release();
    }

    return nullptr;
}

}// namespace atlas
