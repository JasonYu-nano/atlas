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
    dll_handle_ = PlatformTraits::LoadDynamicLibrary({"opengl32.dll"});
    if (dll_handle_)
    {
        WglCreateContext = reinterpret_cast<HGLRC (WINAPI *)(HDC)>(ResolveGLSymbol("wglCreateContext"));
        WglDeleteContext = reinterpret_cast<BOOL (WINAPI *)(HGLRC)>(ResolveGLSymbol("wglDeleteContext"));
        WglGetCurrentContext = reinterpret_cast<HGLRC (WINAPI *)()>(ResolveGLSymbol("wglGetCurrentContext"));
        WglGetCurrentDC = reinterpret_cast<HDC (WINAPI *)()>(ResolveGLSymbol("wglGetCurrentDC"));
        WglGetProcAddress = reinterpret_cast<PROC (WINAPI *)(LPCSTR)>(ResolveGLSymbol("wglGetProcAddress"));
        WglMakeCurrent = reinterpret_cast<BOOL (WINAPI *)(HDC, HGLRC)>(ResolveGLSymbol("wglMakeCurrent"));
        WglShareLists = reinterpret_cast<BOOL (WINAPI *)(HGLRC, HGLRC)>(ResolveGLSymbol("wglShareLists"));
        WglSwapBuffers = reinterpret_cast<BOOL (WINAPI *)(HDC)>(ResolveGLSymbol("wglSwapBuffers"));
        WglSetPixelFormat = reinterpret_cast<BOOL (WINAPI *)(HDC, int, const PIXELFORMATDESCRIPTOR *)>(ResolveGLSymbol("wglSetPixelFormat"));
        WglDescribePixelFormat = reinterpret_cast<int (WINAPI *)(HDC, int, UINT, PIXELFORMATDESCRIPTOR *)>(ResolveGLSymbol("wglDescribePixelFormat"));

        GlGetError = reinterpret_cast<GLenum (APIENTRY *)()>(ResolveGLSymbol("glGetError"));
        GlGetIntegerv = reinterpret_cast<void (APIENTRY *)(GLenum , GLint *)>(ResolveGLSymbol("glGetIntegerv"));
        GlGetString = reinterpret_cast<const GLubyte * (APIENTRY *)(GLenum )>(ResolveGLSymbol("glGetString"));
    }
}

WindowsOpenGL32::~WindowsOpenGL32()
{
    if (dll_handle_)
    {
        PlatformTraits::FreeDynamicLibrary(dll_handle_);
        dll_handle_ = nullptr;
    }
}

WindowGLStaticContext::WindowGLStaticContext()
{
    dll_handle_ = PlatformTraits::LoadDynamicLibrary({"opengl32.dll"});
    if (dll_handle_)
    {
        wgl_create_context_ = reinterpret_cast<FnWglCreateContext>(ResolveSymbol("wglCreateContext"));
        wgl_delete_context_ = reinterpret_cast<FnWglDeleteContext>(ResolveSymbol("wglDeleteContext"));
        wgl_get_current_context_ = reinterpret_cast<FnWglGetCurrentContext>(ResolveSymbol("wglGetCurrentContext"));
        wgl_get_current_dc_ = reinterpret_cast<FnWglGetCurrentDC>(ResolveSymbol("wglGetCurrentDC"));
        wgl_get_proc_address_ = reinterpret_cast<FnWglGetProcAddress>(ResolveSymbol("wglGetProcAddress"));
        wgl_make_current_ = reinterpret_cast<FnWglMakeCurrent>(ResolveSymbol("wglMakeCurrent"));
        wgl_share_lists_ = reinterpret_cast<FnWglShareLists>(ResolveSymbol("wglShareLists"));
        wgl_swap_buffers_ = reinterpret_cast<BOOL (WINAPI *)(HDC)>(ResolveSymbol("wglSwapBuffers"));
        wgl_set_pixel_format_ = reinterpret_cast<BOOL (WINAPI *)(HDC, int, const PIXELFORMATDESCRIPTOR *)>(ResolveSymbol("wglSetPixelFormat"));
        wgl_describe_pixel_format_ = reinterpret_cast<int (WINAPI *)(HDC, int, UINT, PIXELFORMATDESCRIPTOR *)>(ResolveSymbol("wglDescribePixelFormat"));

        gl_get_error_ = reinterpret_cast<FnGlGetError>(ResolveSymbol("glGetError"));
        gl_get_integerv_ = reinterpret_cast<FnGlGetIntegerv>(ResolveSymbol("glGetIntegerv"));
        gl_get_string_ = reinterpret_cast<FnGlGetString>(ResolveSymbol("glGetString"));

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
        PlatformTraits::FreeDynamicLibrary(dll_handle_);
        dll_handle_ = nullptr;
    }
}

String WindowGLStaticContext::GetGLString(uint32 which) const
{
    if (gl_get_string_)
    {
        if (const GLubyte* s = gl_get_string_(which))
        {
            return String(reinterpret_cast<const char*>(s));
        }
    }
    return {};
}

String WindowGLStaticContext::GetGLError() const
{
    if (gl_get_string_)
    {
        return GetGLString(gl_get_error_());
    }
    return {};
}

WindowsGLContext::WindowsGLContext()
{
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

    std::shared_ptr<ApplicationWindow> window = app->MakeDummyWindow();
    if (!window)
    {
        return;
    }

    HWND hwnd = static_cast<HWND>(window->get_native_handle());
    if (!hwnd)
    {
        return;
    }

    HDC hdc = ::GetDC(hwnd);
    if (!hdc)
    {
        return;
    }

    auto guard_dc = on_scope_exit([]{
        ::ReleaseDC(hwnd, hdc);
    });

    if (static_context_.SupportExtension() && false)
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
        obtained_pixel_format_descriptor_.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_GENERIC_FORMAT;
        obtained_pixel_format_descriptor_.iPixelType = PFD_TYPE_RGBA;

        // Use the GDI functions. Under the hood this will call the wgl variants in opengl32.dll.
        pixel_format_ = ChoosePixelFormat(hdc, &obtained_pixel_format_descriptor_);
    }

    if (!pixel_format_)
    {
        LOG_WARN(rhi, "unable find a suitable pixel format.");
        return;
    }

    if (!SetPixelFormat(hdc, pixel_format_, &obtained_pixel_format_descriptor_))
    {
        LOG_WARN(rhi, "SetPixelFormat failed.");
        return;
    }

    render_context_ = static_context_.wgl_create_context_(hdc);
    if (!render_context_)
    {
        LOG_WARN(rhi, "unable to create a gl Context.");
        return;
    }

    LOG_INFO(rhi, "OpenGL version: {0}", static_context_.GetGLString(GL_VERSION));
}

WindowsGLContext::~WindowsGLContext()
{
    for (auto& [hwnd, hdc, ctx] : window_contexts_)
    {
        ReleaseDC(hwnd, hdc);
    }
    window_contexts_.Clear();

    if (render_context_)
    {
        wglDeleteContext(render_context_);
        render_context_ = nullptr;
    }
}

bool WindowsGLContext::make_current(const ApplicationWindow& window)
{
    HWND hwnd = static_cast<HWND>(window.get_native_handle());
    if (!hwnd)
    {
        return false;
    }

    //TODO: check if it's already the current context

    size_t idx = window_contexts_.Add({hwnd, GetDC(hwnd), render_context_});
    const ContextInfo& info = window_contexts_[idx];
    const bool result = static_context_.wgl_make_current_(info.hdc, render_context_);
    if (!result)
    {
        LOG_ERROR(rhi, "opengl error: {0}", static_context_.GetGLError());
    }
    return result;
}

}// namespace atlas
