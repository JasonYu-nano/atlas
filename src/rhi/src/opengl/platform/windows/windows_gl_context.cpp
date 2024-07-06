// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "platform/windows/windows_gl_context.hpp"

#include "application_module.hpp"
#include "utility/on_scope_exit.hpp"
#include "module/module_manager.hpp"
#include "rhi_error_code.hpp"

namespace atlas
{

static PIXELFORMATDESCRIPTOR pixel_format_from_surface_format(const SurfaceFormat& format,
                                  const WindowsOpenGLAdditionalFormat& additional)
{
    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.iLayerType  = PFD_MAIN_PLANE;
    pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_SUPPORT_COMPOSITION;
    const bool is_pixmap = test_flags(additional.format_flags, EWindowsGLFormatFlags::RenderToPixmap);
    pfd.dwFlags |= is_pixmap ? PFD_DRAW_TO_BITMAP : PFD_DRAW_TO_WINDOW;
    if (!test_flags(additional.format_flags, EWindowsGLFormatFlags::DirectRendering))
    {
        pfd.dwFlags |= PFD_GENERIC_FORMAT;
    }

    if (format.stereo())
    {
        pfd.dwFlags |= PFD_STEREO;
    }

    if (format.swap_behavior() != ESwapBehavior::SingleBuffer && !is_pixmap)
    {
        pfd.dwFlags |= PFD_DOUBLEBUFFER;
    }
    pfd.cDepthBits = format.depth_buffer_size() >= 0 ? format.depth_buffer_size() : 32;
    pfd.cRedBits = format.red_buffer_size() > 0 ? format.red_buffer_size() : 8;
    pfd.cGreenBits = format.green_buffer_size() > 0 ? format.green_buffer_size() : 8;
    pfd.cBlueBits = format.blue_buffer_size() > 0 ? format.blue_buffer_size() : 8;
    pfd.cAlphaBits = format.alpha_buffer_size() > 0 ? format.alpha_buffer_size() : 8;
    pfd.cStencilBits = format.stencil_buffer_size() > 0 ? format.stencil_buffer_size() : 8;
    if (test_flags(additional.format_flags, EWindowsGLFormatFlags::AccumBuffer))
    {
        pfd.cAccumRedBits = pfd.cAccumGreenBits = pfd.cAccumBlueBits = pfd.cAccumAlphaBits = 16;
    }
    return pfd;
}

static bool is_direct_rendering(const PIXELFORMATDESCRIPTOR &pfd)
{
    return (pfd.dwFlags & PFD_GENERIC_ACCELERATED) || !(pfd.dwFlags & PFD_GENERIC_FORMAT);
}

static bool has_overlay(const PIXELFORMATDESCRIPTOR &pd)
{
    return (pd.bReserved & 0x0f) != 0;
}

static bool is_acceptable_format(const WindowsOpenGLAdditionalFormat& additional, const PIXELFORMATDESCRIPTOR& pfd, bool ignore_gl_support = false) // ARB format may not contain it.
{
    const bool pixmap_requested = test_flags(additional.format_flags, EWindowsGLFormatFlags::RenderToPixmap);
    if (pixmap_requested && (pfd.dwFlags & PFD_DRAW_TO_BITMAP) == 0)
    {
        return false;
    }

    if (pixmap_requested && pfd.cColorBits != additional.pixmap_depth)
    {
        return false;
    }

    if (!ignore_gl_support && (pfd.dwFlags & PFD_SUPPORT_OPENGL) == 0)
    {
        return false;
    }

    if (test_flags(additional.format_flags, EWindowsGLFormatFlags::Overlay) != has_overlay(pfd))
    {
        return false;
    }

    return true;
}

namespace gdi
{

static int choose_pixel_format(HDC hdc, const SurfaceFormat& format, const WindowsOpenGLAdditionalFormat& additional, PIXELFORMATDESCRIPTOR& pfd)
{
    std::memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;

    PIXELFORMATDESCRIPTOR request_pfd = pixel_format_from_surface_format(format, additional);
    int32 pixel_format = ChoosePixelFormat(hdc, &request_pfd);
    if (pixel_format > 0)
    {
        DescribePixelFormat(hdc, pixel_format, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
        if (is_acceptable_format(additional, pfd))
        {
            return pixel_format;
        }
    }

    const int pfi_max = DescribePixelFormat(hdc, 0, 0, nullptr);
    int best_score = -1;
    int best_pfi = -1;
    const bool stereo_requested = format.stereo();
    const bool accum_buffer_requested = test_flags(additional.format_flags, EWindowsGLFormatFlags::AccumBuffer);
    const bool double_buffer_requested = format.swap_behavior() == ESwapBehavior::DoubleBuffer;
    const bool direct_rendering_requested = test_flags(additional.format_flags, EWindowsGLFormatFlags::DirectRendering);
    for (int pfi = 1; pfi <= pfi_max; pfi++)
    {
        PIXELFORMATDESCRIPTOR check_pfd = {};
        check_pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        check_pfd.nVersion = 1;

        DescribePixelFormat(hdc, pfi, sizeof(PIXELFORMATDESCRIPTOR), &check_pfd);
        if (is_acceptable_format(additional, check_pfd))
        {
            int32 score = check_pfd.cColorBits + check_pfd.cAlphaBits + check_pfd.cStencilBits;
            if (accum_buffer_requested)
            {
                score += check_pfd.cAccumBits;
            }
            if (double_buffer_requested == (check_pfd.dwFlags & PFD_DOUBLEBUFFER) != 0)
            {
                score += 1000;
            }
            if (stereo_requested == ((check_pfd.dwFlags & PFD_STEREO) != 0))
            {
                score += 2000;
            }
            if (direct_rendering_requested == is_direct_rendering(check_pfd))
            {
                score += 4000;
            }
            if (check_pfd.iPixelType == PFD_TYPE_RGBA)
            {
                score += 8000;
            }
            if (score > best_score)
            {
                best_score = score;
                best_pfi = pfi;
                pfd = check_pfd;
            }
            LOG_INFO(rhi, "Checking pixel format {0} / {1}. Current score: {2}, best score: {3}, best pixel format {4}", pfi, pfi_max, score, best_score, best_pfi);
        }
    }
    if (best_pfi > 0)
    {
        pixel_format = best_pfi;
    }
    return pixel_format;
}

}// namespace gdi

class GLTemporaryContext
{
public:
    GLTemporaryContext(WindowGLStaticContext& static_context)
        : static_context_(&static_context)
        , previous({nullptr, static_context.wgl_get_current_dc_(), static_context.wgl_get_current_context_()})
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

        dummy_window_ = app->make_dummy_window();
        if (!dummy_window_)
        {
            return;
        }

        current.hwnd = static_cast<HWND>(dummy_window_->native_handle());
        if (!current.hwnd)
        {
            return;
        }

        current.hdc = ::GetDC(current.hwnd);
        current.ctx = create_temporary_context(*static_context_, current.hdc);
        static_context_->wgl_make_current_(current.hdc, current.ctx);
    }

    ~GLTemporaryContext()
    {
        static_context_->wgl_make_current_(previous.hdc, previous.ctx);
        static_context_->wgl_delete_context_(current.ctx);
        ::ReleaseDC(current.hwnd, current.hdc);
        dummy_window_.reset();
    }

    static HGLRC create_temporary_context(WindowGLStaticContext& static_context, HDC hdc)
    {
        HGLRC rc = nullptr;
        if (hdc)
        {
            PIXELFORMATDESCRIPTOR pfd{};
            pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
            pfd.nVersion = 1;
            pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_GENERIC_FORMAT;
            pfd.iPixelType = PFD_TYPE_RGBA;

            const int pixel_format = ChoosePixelFormat(hdc, &pfd);
            if (!pixel_format)
            {
                return nullptr;
            }
            if (!SetPixelFormat(hdc, pixel_format, &pfd))
            {
                return nullptr;
            }
            rc = static_context.wgl_create_context_(hdc);
        }
        return rc;
    }

private:
    WindowGLStaticContext* static_context_;
    const WindowsGLContext::ContextInfo previous;
    WindowsGLContext::ContextInfo current;
    std::shared_ptr<ApplicationWindow> dummy_window_;
};

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

        gl_get_error_ = reinterpret_cast<FnGlGetError>(resolve_symbol("glGetError"));
        gl_get_integerv_ = reinterpret_cast<FnGlGetIntegerv>(resolve_symbol("glGetIntegerv"));
        gl_get_string_ = reinterpret_cast<FnGlGetString>(resolve_symbol("glGetString"));

        ASSERT(wgl_get_proc_address_);

        // We need a current context for wgl_get_proc_address_()/gl_get_string_() to work.
        GLTemporaryContext guard_temporary_context(*this);
        wgl_get_pixel_format_attrib_ivarb_ = reinterpret_cast<FnWglGetPixelFormatAttribIVARB>(wgl_get_proc_address_("wglGetPixelFormatAttribivARB"));
        wgl_choose_pixel_format_arb_ = reinterpret_cast<FnWglChoosePixelFormatARB>(wgl_get_proc_address_("wglGetPixelFormatAttribivARB"));
        wgl_create_context_attribs_arb_ = reinterpret_cast<FnWglCreateContextAttribsARB>(wgl_get_proc_address_("wglCreateContextAttribsARB"));
        wgl_swap_internal_ext_ = reinterpret_cast<FnWglSwapInternalExt>(wgl_get_proc_address_("wglSwapIntervalEXT"));
        wgl_get_swap_internal_ext_ = reinterpret_cast<FnWglGetSwapInternalExt>(wgl_get_proc_address_("wglGetSwapIntervalEXT"));
        wgl_get_extensions_string_arb_ = reinterpret_cast<FnWglGetExtensionsStringARB>(wgl_get_proc_address_("wglGetExtensionsStringARB"));

        LOG_INFO(rhi, "OpenGL version: {0}", get_gl_string(GL_VERSION));
    }
    else
    {
        // Can not find opengl32 dll, maybe the os does not support hardware rendering.
        // Just notify user and exit engine.
        PlatformTraits::show_message_box(ESystemMsgBoxType::Ok, "Fatal Error", "The operating system seems does not support the required OpenGL version");
        std::exit(error_code::opengl_enviroment_required);
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

WindowsGLContext::WindowsGLContext(WindowGLStaticContext& static_context, const SurfaceFormat& request_format, const WindowsOpenGLAdditionalFormat& additional)
    : static_context_(&static_context)
{
    ERenderableType renderable_type = request_format.renderable_type();
    if (renderable_type == ERenderableType::Default)
    {
        renderable_type = ERenderableType::OpenGL;
    }
    if (renderable_type != ERenderableType::OpenGL)
    {
        return;
    }

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

    if (static_context_->support_extension() && false)
    {
        if (static_context_->wgl_get_extensions_string_arb_)
        {
            const char* exts = static_context_->wgl_get_extensions_string_arb_(hdc);
            LOG_INFO(rhi, "wgl extensions:{0}", exts);
        }
        int32 fmt = 0;
        uint32 num_fmt = 0;
        bool valid = static_context_->wgl_choose_pixel_format_arb_(hdc, nullptr, nullptr, 1, &fmt, &num_fmt) && num_fmt > 0; // TODO:
        if (valid)
        {
            pixel_format_ = fmt;
            //obtained_pixel_format_descriptor_ = static_context_.wgl_get_pixel_format_attrib_ivarb_(hdc, pixel_format_, 0, 0, nullptr, nullptr);
        }
    }

    if (!pixel_format_)
    {
        // Use the GDI functions. Under the hood this will call the wgl variants in opengl32.dll.
        pixel_format_ = gdi::choose_pixel_format(hdc, request_format, additional, obtained_pixel_format_descriptor_);
    }

    if (!pixel_format_)
    {
        LOG_WARN(rhi, "Unable find a suitable pixel format.");
        return;
    }

    if (!SetPixelFormat(hdc, pixel_format_, &obtained_pixel_format_descriptor_))
    {
        LOG_WARN(rhi, "SetPixelFormat failed.");
        return;
    }

    render_context_ = static_context_->wgl_create_context_(hdc);
    if (!render_context_)
    {
        LOG_WARN(rhi, "Unable to create a gl Context.");
        return;
    }
}

WindowsGLContext::~WindowsGLContext()
{
    WindowsGLContext::done_current();

    if (render_context_)
    {
        static_context_->wgl_delete_context_(render_context_);
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
        if (static_context_->wgl_get_current_context_() == context_info->ctx && static_context_->wgl_get_current_dc_() == context_info->hdc)
        {
            return true;
        }
        const bool success = static_context_->wgl_make_current_(context_info->hdc, context_info->ctx);
        if (!success)
        {
            LOG_WARN(rhi, "wgl_make_current_() failed for existing context info");
        }
        return success;
    }

    const HDC hdc = GetDC(hwnd);

    if (!window.has_flag(EWindowFlag::OpenGlPixelFormatInitialized))
    {
        if (!SetPixelFormat(hdc, pixel_format_, &obtained_pixel_format_descriptor_))
        {
            ReleaseDC(hwnd, hdc);
            return false;
        }
        window.set_flag(EWindowFlag::OpenGlPixelFormatInitialized);
    }

    size_t idx = window_contexts_.add({hwnd, hdc, render_context_});
    const ContextInfo& info = window_contexts_[idx];
    const bool result = static_context_->wgl_make_current_(info.hdc, render_context_);
    CLOG_ERROR(!result, rhi, "OpenGL error: {0}", static_context_->get_gl_error());
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

void WindowsGLContext::done_current()
{
    static_context_->wgl_make_current_(nullptr, nullptr);
    for (auto& [hwnd, hdc, ctx] : window_contexts_)
    {
        ReleaseDC(hwnd, hdc);
    }
    window_contexts_.clear();
}

WindowsGLContext::ContextInfo* WindowsGLContext::find_context(HWND hwnd)
{
    size_t index = window_contexts_.find([=](auto&& elem) {
        return elem.hwnd == hwnd;
    });

    return index == INDEX_NONE ? nullptr : &window_contexts_[index];
}

}// namespace atlas