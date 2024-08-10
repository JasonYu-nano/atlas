#include "draw_triangle.hpp"

#include "application_module.hpp"
#include "module/module_manager.hpp"
#include "utility/on_scope_exit.hpp"

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

    current_window_ = app->get_key_window();
    if (current_window_)
    {
        if (ctx_->make_current(*current_window_))
        {
            if (!setup_shaders(shader_program_))
            {
                return;
            }

            float vertices[] = {
                0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
                0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
                -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
                -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f,
            };

            uint32 indices[] = {
                0, 1, 3,
                1, 2, 3
            };

            ctx_->functions()->glGenVertexArrays(1, &vertext_array_object_);
            ctx_->functions()->glBindVertexArray(vertext_array_object_);

            ctx_->functions()->glGenBuffers(1, &vertex_buffer_object_);
            ctx_->functions()->glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
            ctx_->functions()->glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            ctx_->functions()->glGenBuffers(1, &element_buffer_object_);
            ctx_->functions()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object_);
            ctx_->functions()->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

            ctx_->functions()->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
            ctx_->functions()->glEnableVertexAttribArray(0);

            ctx_->functions()->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
            ctx_->functions()->glEnableVertexAttribArray(1);

            ctx_->functions()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            ctx_->functions()->glBindBuffer(GL_ARRAY_BUFFER, 0);
            ctx_->functions()->glBindVertexArray(0);

            ready_for_tick_ = true;
        }
    }
}

void DrawTriangle::tick(float delta_time)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw our first triangle
    ctx_->functions()->glUseProgram(shader_program_);

    // auto current_time = duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
    // float green_value = sin(static_cast<float>(current_time.count())) / 2.0f + 0.5f;
    // int vertexColorLocation = ctx_->functions()->glGetUniformLocation(shader_program_, "our_color");
    // ctx_->functions()->glUniform4f(vertexColorLocation, 0.0f, green_value, 0.0f, 1.0f);

    ctx_->functions()->glBindVertexArray(vertext_array_object_); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    // glDrawArrays(GL_TRIANGLES, 0, 3);
    ctx_->functions()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object_);
    ctx_->functions()->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    //ctx_->functions()->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    ctx_->swap_buffers(*current_window_);
}

void DrawTriangle::deinitialize()
{
    ctx_->functions()->glDeleteVertexArrays(1, &vertext_array_object_);
    ctx_->functions()->glDeleteBuffers(1, &vertex_buffer_object_);
    if (shader_program_ != 0)
    {
        ctx_->functions()->glDeleteProgram(shader_program_);
    }
}

bool DrawTriangle::setup_shaders(uint32& shader_program)
{
    int32 success;
    char log_info[512];

    const char *vertexShaderSource = R"(#version 330 core
                layout (location = 0) in vec3 pos3;
                layout (location = 1) in vec3 color3;

                out vec3 user_color;
                void main()
                {
                    gl_Position = vec4(pos3, 1.0);
                    user_color = color3;
                })";

    uint32 vertex_shader;
    vertex_shader = ctx_->functions()->glCreateShader(GL_VERTEX_SHADER);

    auto guard_vs = on_scope_exit([=]() {
        ctx_->functions()->glDeleteShader(vertex_shader);
    });

    ctx_->functions()->glShaderSource(vertex_shader, 1, &vertexShaderSource, NULL);
    ctx_->functions()->glCompileShader(vertex_shader);

    // check compilation result.
    ctx_->functions()->glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        ctx_->functions()->glGetShaderInfoLog(vertex_shader, 512, NULL, log_info);
        LOG_WARN(temp, "Vertext shader compilation failed: {}", log_info);
        return false;
    }

    const char* fragment_shader_source = R"(#version 330 core
                in vec3 user_color;
                out vec4 frag_color;
                uniform vec4 our_color;
                void main()
                {
                    //frag_color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
                    frag_color = vec4(user_color, 1.0);
                } )";

    uint32 fragment_shader;
    fragment_shader = ctx_->functions()->glCreateShader(GL_FRAGMENT_SHADER);

    auto guard_fs = on_scope_exit([=]() {
        ctx_->functions()->glDeleteShader(fragment_shader);
    });

    ctx_->functions()->glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    ctx_->functions()->glCompileShader(fragment_shader);

    // check compilation result.
    ctx_->functions()->glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        ctx_->functions()->glGetShaderInfoLog(vertex_shader, 512, NULL, log_info);
        LOG_WARN(temp, "Fragment shader compilation failed: {}", log_info);
        return false;
    }

    shader_program = ctx_->functions()->glCreateProgram();
    auto guard_pg = on_scope_exit([=]() {
        ctx_->functions()->glDeleteProgram(shader_program);
    });

    ctx_->functions()->glAttachShader(shader_program, vertex_shader);
    ctx_->functions()->glAttachShader(shader_program, fragment_shader);
    ctx_->functions()->glLinkProgram(shader_program);

    // check for linking errors
    ctx_->functions()->glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        ctx_->functions()->glGetProgramInfoLog(shader_program, 512, NULL, log_info);
        LOG_WARN(temp, "Shader link failed: {}", log_info);
        shader_program = 0;
        return false;
    }

    guard_pg.cancel();

    return true;
}

}// namespace atlas