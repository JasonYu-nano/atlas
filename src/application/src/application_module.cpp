// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "application_module.hpp"

namespace atlas
{

IMPLEMENT_MODULE(ApplicationModule, "application");

void ApplicationModule::Startup()
{
    application_ = PlatformApplication::Create();
    application_->Initialize();
}

void ApplicationModule::Shutdown()
{
    application_->Deinitialize();
}

void ApplicationModule::Tick(float delta_time)
{
    application_->Tick(delta_time);
}

}// namespace atlas