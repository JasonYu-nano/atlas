// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "application_module.hpp"

namespace atlas
{

IMPLEMENT_MODULE(ApplicationModule, "application");

void ApplicationModule::startup()
{
    application_ = std::make_unique<PlatformApplication>();
    application_->Initialize();
}

void ApplicationModule::shutdown()
{
    application_->DeInitialize();
}

void ApplicationModule::tick(float delta_time)
{
    application_->Tick(delta_time);
}

}// namespace atlas