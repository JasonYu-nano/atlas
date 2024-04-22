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

    WindowDescription desc;
    desc.title = "Atlas Game";
    application_->MakeWindow(desc);
}

void ApplicationModule::Shutdown()
{
    application_->Deinitialize();
    application_.reset();
}

void ApplicationModule::Tick(float delta_time)
{
    application_->Tick(delta_time);
}

}// namespace atlas