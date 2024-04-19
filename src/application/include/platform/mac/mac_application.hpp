// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "application_implement.hpp"

namespace atlas
{

class APPLICATION_API MacApplication : public ApplicationImplement
{
public:
    void Initialize() override;
    void Deinitialize() override;
    void Tick(float delta_time) override;
};

} // namespace atlas