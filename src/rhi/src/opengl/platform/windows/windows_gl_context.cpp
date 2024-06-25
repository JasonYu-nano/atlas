// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "platform/windows/windows_gl_context.hpp"

#include "application_module.hpp"
#include "misc/on_scope_exit.hpp"
#include "module/module_manager.hpp"
#include "rhi_log.hpp"

namespace atlas
{

WindowsOpenGL32::WindowsOpenGL32()
{
    dll_handle_ = PlatformTraits::load_library({"opengl32.dll"});
    if (dll_handle_)
    {
        WglCreateContext = reinterpret_cast<HGLRC (WINAPI *)(HDC)>(resolve_gl_symbol("wglCreateContext"));
        WglDeleteContext = reinterpret_cast<BOOL (WINAPI *)(HGLRC)>(resolve_gl_symbol("wglDeleteContext"));
        WglGetCurrentContext = reinterpret_cast<HGLRC (WINAPI *)()>(resolve_gl_symbol("wglGetCurrentContext"));
        WglGetCurrentDC = reinterpret_cast<HDC (WINAPI *)()>(resolve_gl_symbol("wglGetCurrentDC"));
        WglGetProcAddress = reinterpret_cast<PROC (WINAPI *)(LPCSTR)>(resolve_gl_symbol("wglGetProcAddress"));
        WglMakeCurrent = reinterpret_cast<BOOL (WINAPI *)(HDC, HGLRC)>(resolve_gl_symbol("wglMakeCurrent"));
        WglShareLists = reinterpret_cast<BOOL (WINAPI *)(HGLRC, HGLRC)>(resolve_gl_symbol("wglShareLists"));
        WglSwapBuffers = reinterpret_cast<BOOL (WINAPI *)(HDC)>(resolve_gl_symbol("wglSwapBuffers"));
        WglSetPixelFormat = reinterpret_cast<BOOL (WINAPI *)(HDC, int, const PIXELFORMATDESCRIPTOR *)>(resolve_gl_symbol("wglSetPixelFormat"));
        WglDescribePixelFormat = reinterpret_cast<int (WINAPI *)(HDC, int, UINT, PIXELFORMATDESCRIPTOR *)>(resolve_gl_symbol("wglDescribePixelFormat"));

        GlGetError = reinterpret_cast<GLenum (APIENTRY *)()>(resolve_gl_symbol("glGetError"));
        GlGetIntegerv = reinterpret_cast<void (APIENTRY *)(GLenum , GLint *)>(resolve_gl_symbol("glGetIntegerv"));
        GlGetString = reinterpret_cast<const GLubyte * (APIENTRY *)(GLenum )>(resolve_gl_symbol("glGetString"));
    }
}

WindowsOpenGL32::~WindowsOpenGL32()
{
    if (dll_handle_)
    {
        PlatformTraits::free_library(dll_handle_);
        dll_handle_ = nullptr;
    }
}

WindowGLStaticContext::WindowGLStaticContext()
{
    dll_handle_ = PlatformTraits::load_library({"opengl32.dll"});
    if (dll_handle_)
    {
        wgl_create_context_ = reinterpret_cast<FnWglCreateContext>(resolve_symbol("wglCreateContext"));
        wgl_delete_context_ = reinterpret_cast<FnWglDeleteContext>(resolve_symbol("wglDeleteContext"));
        wgl_get_current_context_ = reinterpret_cast<FnWglGetCurrentContext>(resolve_symbol("wglGetCurrentContext"));
        wgl_get_current_dc_ = reinterpret_cast<FnWglGetCurrentDC>(resolve_symbol("wglGetCurrentDC"));
        wgl_get_proc_address_ = reinterpret_cast<FnWglGetProcAddress>(resolve_symbol("wglGetProcAddress"));
        wgl_make_current_ = reinterpret_cast<FnWglMakeCurrent>(resolve_symbol("wglMakeCurrent"));
        wgl_share_lists_ = reinterpret_cast<FnWglShareLists>(resolve_symbol("wglShareLists"));
        wgl_swap_buffers_ = reinterpret_cast<BOOL (WINAPI *)(HDC)>(resolve_symbol("wglSwapBuffers"));
        wgl_set_pixel_format_ = reinterpret_cast<BOOL (WINAPI *)(HDC, int, const PIXELFORMATDESCRIPTOR *)>(resolve_symbol("wglSetPixelFormat"));
        wgl_describe_pixel_format_ = reinterpret_cast<int (WINAPI *)(HDC, int, UINT, PIXELFORMATDESCRIPTOR *)>(resolve_symbol("wglDescribePixelFormat"));

        gl_get_error_ = reinterpret_cast<FnGlGetError>(resolve_symbol("glGetError"));
        gl_get_integerv_ = reinterpret_cast<FnGlGetIntegerv>(resolve_symbol("glGetIntegerv"));
        gl_get_string_ = reinterpret_cast<FnGlGetString>(resolve_symbol("glGetString"));

        if (wgl_get_proc_address_)
        {
            wgl_get_pixel_format_attrib_ivarb_ = reinterpret_cast<FnWglGetPixelFormatAttribIVARB>(wgl_get_proc_address_("wglGetPixelFormatAttribIVARB"));
            wgl_choose_pixel_format_arb_ = reinterpret_cast<FnWglChoosePixelFormatARB>(wgl_get_proc_address_("wglGetPixelFormatAttribivARB"));
            wgl_create_context_attribs_arb_ = reinterpret_cast<FnWglCreateContextAttribsARB>(wgl_get_proc_address_("wglCreateContextAttribsARB"));
            wgl_swap_internal_ext_ = reinterpret_cast<FnWglSwapInternalExt>(wgl_get_proc_address_("wglSwapInternalExt"));
            wgl_get_extensions_string_arb_ = reinterpret_cast<FnWglGetExtensionsStringARB>(wgl_get_proc_address_("wglGetExtensionsStringARB"));
        }
    }
}

WindowGLStaticContext::~WindowGLStaticContext()
{
    if (dll_handle_)
    {
        PlatformTraits::free_library(dll_handle_);
        dll_handle_ = nullptr;
    }
}

String WindowGLStaticContext::get_gl_string(uint32 which) const
{
    if (gl_get_string_)
    {
        if (const GLubyte* s = gl_get_string_(which))
        {
            return {reinterpret_cast<const char*>(s)};
        }
    }
    return {};
}

String WindowGLStaticContext::get_gl_error() const
{
    if (gl_get_string_)
    {
        return get_gl_string(gl_get_error_());
    }
    return {};
}

WindowsGLContext::WindowsGLContext()
{
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

    std::shared_ptr<ApplicationWindow> window = app->make_dummy_window();
    if (!window)
    {
        return;
    }

    HWND hwnd = static_cast<HWND>(window->native_handle());
    if (!hwnd)
    {
        return;
    }

    HDC hdc = ::GetDC(hwnd);
    if (!hdc)
    {
        return;
    }

    auto guard_dc = on_scope_exit([=]{
        ::ReleaseDC(hwnd, hdc);
    });

    if (static_context_.support_extension() && false)
    {
        if (static_context_.wgl_get_extensions_string_arb_)
        {
            const char* exts = static_context_.wgl_get_extensions_string_arb_(hdc);
            LOG_INFO(rhi, "wgl extensions:{0}", exts);
        }
        int32 fmt = 0;
        uint32 num_fmt = 0;
        bool valid = static_context_.wgl_choose_pixel_format_arb_(hdc, nullptr, nullptr, 1, &fmt, &num_fmt) && num_fmt > 0; // TODO:
        if (valid)
        {
            pixel_format_ = fmt;
            //obtained_pixel_format_descriptor_ = static_context_.wgl_get_pixel_format_attrib_ivarb_(hdc, pixel_format_, 0, 0, nullptr, nullptr);
        }
    }

    if (!pixel_format_)
    {
        std::memset(&obtained_pixel_format_descriptor_, 0, sizeof(PIXELFORMATDESCRIPTOR));
        obtained_pixel_format_descriptor_.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        obtained_pixel_format_descriptor_.nVersion = 1;
        obtained_pixel_format_descriptor_.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_GENERIC_FORMAT | PFD_DOUBLEBUFFER;
        obtained_pixel_format_descriptor_.iPixelType = PFD_TYPE_RGBA;
        obtained_pixel_format_descriptor_.cColorBits = 24;
        obtained_pixel_format_descriptor_.cDepthBits = 16;
        obtained_pixel_format_descriptor_.iLayerType = PFD_MAIN_PLANE;

        // Use the GDI functions. Under the hood this will call the wgl variants in opengl32.dll.
        pixel_format_ = ChoosePixelFormat(hdc, &obtained_pixel_format_descriptor_);

        if (pixel_format_ >= 0)
        {
            static_context_.wgl_describe_pixel_format_(hdc, pixel_format_, sizeof(PIXELFORMATDESCRIPTOR), &obtained_pixel_format_descriptor_);
        }
    }

    if (!pixel_format_)
    {
        LOG_WARN(rhi, "Unable find a suitable pixel format.");
        return;
    }

    if (!SetPixelFormat(hdc, pixel_format_, &obtained_pixel_format_descriptor_)) //TODO:
    {
        LOG_WARN(rhi, "SetPixelFormat failed.");
        return;
    }

    render_context_ = static_context_.wgl_create_context_(hdc);
    if (!render_context_)
    {
        LOG_WARN(rhi, "Unable to create a gl Context.");
        return;
    }
    const bool result = static_context_.wgl_make_current_(hdc, render_context_);
    LOG_INFO(rhi, "OpenGL version: {0}", static_context_.get_gl_string(GL_VERSION));
}

WindowsGLContext::~WindowsGLContext()
{
    //static_context_.wgl_make_current_(info.hdc, nullptr);
    for (auto& [hwnd, hdc, ctx] : window_contexts_)
    {
        ReleaseDC(hwnd, hdc);
    }
    window_contexts_.clear();

    if (render_context_)
    {
        static_context_.wgl_delete_context_(render_context_);
        render_context_ = nullptr;
    }
}

bool WindowsGLContext::make_current(ApplicationWindow& window)
{
    HWND hwnd = static_cast<HWND>(window.native_handle());
    if (!hwnd)
    {
        return false;
    }
    
    if (const ContextInfo* context_info = find_context(static_cast<HWND>(window.native_handle())))
    {
        if (static_context_.wgl_get_current_context_() == context_info->ctx && static_context_.wgl_get_current_dc_() == context_info->hdc)
        {
            return true;
        }
        const bool success = static_context_.wgl_make_current_(context_info->hdc, context_info->ctx);
        if (!success)
        {
            LOG_WARN(rhi, "wgl_make_current_() failed for existing context info");
        }
        return success;
    }

    const HDC hdc = GetDC(hwnd);

    if (!window.has_flag(EWindowFlag::OpenGlPixelFormatInitialized))
    {
        if (!static_context_.wgl_set_pixel_format_(hdc, pixel_format_, &obtained_pixel_format_descriptor_))
        {
            ReleaseDC(hwnd, hdc);
            return false;
        }
        window.set_flag(EWindowFlag::OpenGlPixelFormatInitialized);
    }

    size_t idx = window_contexts_.add({hwnd, hdc, render_context_});
    const ContextInfo& info = window_contexts_[idx];
    const bool result = static_context_.wgl_make_current_(info.hdc, render_context_);
    if (!result)
    {
        LOG_ERROR(rhi, "OpenGL error: {0}", static_context_.get_gl_error());
    }
    return result;
}

bool WindowsGLContext::swap_buffers(ApplicationWindow& window)
{
    if (const ContextInfo* context_info = find_context(static_cast<HWND>(window.native_handle())))
    {
        SwapBuffers(context_info->hdc);
        return true;
    }
    LOG_WARN(rhi, "Cannot find window {0}", window.native_handle());
    return false;
}

WindowsGLContext::ContextInfo* WindowsGLContext::find_context(HWND hwnd)
{
    size_t index = window_contexts_.find([=](auto&& elem) {
        return elem.hwnd == hwnd;
    });

    return index == INDEX_NONE ? nullptr : &window_contexts_[index];
}

}// namespace atlas
