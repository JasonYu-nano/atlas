// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "application_module.hpp"

namespace atlas
{

IMPLEMENT_MODULE(ApplicationModule, "application");

void ApplicationModule::startup()
{
    application_ = PlatformApplication::Create();
    application_->initialize();

    WindowDescription desc;
    desc.title = "Atlas Game";
    application_->make_window(desc);
}

void ApplicationModule::shutdown()
{
    application_->deinitialize();
    application_.reset();
}

void ApplicationModule::tick(float delta_time)
{
    application_->tick(delta_time);
}

}// namespace atlas