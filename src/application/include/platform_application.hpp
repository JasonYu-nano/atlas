// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <memory>

#include "application_implement.hpp"

namespace atlas
{

class APPLICATION_API PlatformApplication
{
public:
    static std::unique_ptr<PlatformApplication> Create();

    PlatformApplication() = delete;

    void Initialize()
    {
        implement_->Initialize();
    }

    void Deinitialize()
    {
        implement_->Deinitialize();
    }

    void Tick(float delta_time)
    {
        implement_->Tick(delta_time);
    }

private:
    explicit PlatformApplication(ApplicationImplement* implement) : implement_(implement) {}

    std::unique_ptr<ApplicationImplement> implement_{ nullptr };
};

}// namespace atlas