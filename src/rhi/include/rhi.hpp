// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <memory>

#include "rhi_types.hpp"

namespace atlas
{

class RHI_API Rhi
{
public:
    static Rhi* create(ERhiBackend backend, RHIBackendInitParams* params);

private:
    std::unique_ptr<class RhiBackend> backend_{ nullptr };
};

} // namespace atlas