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
    void tick(float delta_time) override;
private:
    OpenGLContext* ctx_{ nullptr };
};

}// namespace atlas