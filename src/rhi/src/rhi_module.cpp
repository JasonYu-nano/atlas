// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "rhi_module.hpp"

#include "application_module.hpp"
#include "opengl_context.hpp"
#include "module/module_manager.hpp"
#include "platform/windows/windows_gl_context.hpp"

namespace atlas
{

IMPLEMENT_MODULE(RHIModule, "rhi");

void RHIModule::startup()
{
    ctx_ = new OpenGLContext();
    ctx_->create();

    auto app_module = static_cast<ApplicationModule*>(ModuleManager::load("application"));
    if (!app_module)
    {
        return;
    }
    PlatformApplication* app = app_module->get_application();
    if (!app)
    {
        return;
    }

    if (auto&& key = app->get_key_window())
    {
        if (ctx_->make_current(*key))
        {

        }
    }
}

void RHIModule::shutdown()
{
    delete ctx_;
    ctx_ = nullptr;
}

void RHIModule::tick(float delta_time)
{
    if (ctx_)
    {
        auto app_module = static_cast<ApplicationModule*>(ModuleManager::load("application"));
        if (app_module)
        {
            PlatformApplication* app = app_module->get_application();
            if (app)
            {
                if (auto&& key = app->get_key_window())
                {
                    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                    glClear(GL_COLOR_BUFFER_BIT);
                    ctx_->swap_buffers(*key);
                }
            }
        }
    }
}

}// namespace atlas
