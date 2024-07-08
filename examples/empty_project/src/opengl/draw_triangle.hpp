#pragma once

#include "core_def.hpp"
#include "opengl_context.hpp"
#include "tickable/tickable_object.hpp"

namespace atlas
{

class DrawTriangle : public TickableObject
{
public:
    DrawTriangle() = default;
    ~DrawTriangle() override;

    void initialize();
    void deinitialize();
private:
    bool can_tick() override
    {
        return ready_for_tick_;
    }

    void tick(float delta_time) override;

    bool setup_shaders(uint32& shader_program);

    bool ready_for_tick_{ false };
    std::shared_ptr<ApplicationWindow> current_window_;
    OpenGLContext* ctx_{ nullptr };
    uint32 shader_program_{ 0 };
    uint32 vertex_buffer_object_{ 0 };
    uint32 element_buffer_object_{ 0 };
    uint32 vertext_array_object_{ 0 };
};

}// namespace atlas