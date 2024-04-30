// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "rhi_module.hpp"

#include "application_module.hpp"
#include "module/module_manager.hpp"
#include "platform/windows/windows_gl_context.hpp"

namespace atlas
{

IMPLEMENT_MODULE(RHIModule, "rhi");

void RHIModule::Startup()
{
    ctx_ = new WindowsGLContext();

    auto app_module = static_cast<ApplicationModule*>(ModuleManager::Load("application"));
    if (!app_module)
    {
        return;
    }
    PlatformApplication* app = app_module->GetApplication();
    if (!app)
    {
        return;
    }

    if (auto&& key = app->GetKeyWindow())
    {
        if (ctx_->MakeCurrent(*key))
        {
            glViewport(0, 0, 800, 600);
        }
    }
}

void RHIModule::Shutdown()
{
    delete ctx_;
    ctx_ = nullptr;
}

void RHIModule::Tick(float delta_time)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

}// namespace atlas
