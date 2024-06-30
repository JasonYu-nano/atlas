#include "draw_triangle.hpp"

#include "application_module.hpp"
#include "module/module_manager.hpp"

namespace atlas
{

DrawTriangle::~DrawTriangle()
{
    delete ctx_;
    ctx_ = nullptr;
}

void DrawTriangle::initialize()
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
            unsigned int vertex_buffer_object;
            // glGenBuffers(1, &vertex_buffer_object);
            // glBindBuffer()
        }
    }
}

void DrawTriangle::tick(float delta_time)
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