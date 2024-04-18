// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "core_def.hpp"

namespace atlas
{

class APPLICATION_API MacApplication
{
public:
    void Initialize();
    void DeInitialize();
    void Tick(float delta_time);
};

using PlatformApplication = MacApplication;

} // namespace atlas