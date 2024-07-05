// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "opengl_types.hpp"
#include "platform/windows/windows_platform_traits.hpp"
#include "platform_gl_context.hpp"
#include "surface_format.hpp"
#include "rhi_log.hpp"

namespace atlas
{

enum class EWindowsGLFormatFlags : uint8
{
    DirectRendering = 1 << 0,
    Overlay         = 1 << 1,
    RenderToPixmap  = 1 << 2,
    AccumBuffer     = 1 << 3,
};

ENUM_BIT_MASK(EWindowsGLFormatFlags);

// Additional format information for Windows.
struct WindowsOpenGLAdditionalFormat
{
    EWindowsGLFormatFlags format_flags = EWindowsGLFormatFlags::DirectRendering;
    uint8 pixmap_depth = 0; // for RenderToPixmap
};

class RHI_API WindowGLStaticContext
{
public:
    typedef HGLRC (WINAPI * FnWglCreateContext)(HDC dc);
    typedef BOOL (WINAPI * FnWglDeleteContext)(HGLRC context);
    typedef HGLRC (WINAPI * FnWglGetCurrentContext)();
    typedef HDC (WINAPI * FnWglGetCurrentDC)();
    typedef PROC (WINAPI * FnWglGetProcAddress)(LPCSTR name);
    typedef BOOL (WINAPI * FnWglMakeCurrent)(HDC dc, HGLRC context);
    typedef BOOL (WINAPI * FnWglShareLists)(HGLRC context1, HGLRC context2);

    typedef BOOL (WINAPI * FnWglSwapBuffers)(HDC dc);
    typedef BOOL (WINAPI * FnWglChoosePixelFormat)(HDC hdc, const PIXELFORMATDESCRIPTOR *pfd);
    typedef BOOL (WINAPI * FnWglSetPixelFormat)(HDC dc, int32 pf, const PIXELFORMATDESCRIPTOR *pfd);
    typedef int (WINAPI * FnWglDescribePixelFormat)(HDC dc, int32 pf, UINT size, PIXELFORMATDESCRIPTOR *pfd);

    typedef GLenum (APIENTRY * FnGlGetError)();
    typedef void (APIENTRY * FnGlGetIntegerv)(GLenum pname, GLint* params);
    typedef const GLubyte * (APIENTRY * FnGlGetString)(GLenum name);

    typedef bool (APIENTRY * FnWglGetPixelFormatAttribIVARB)(HDC hdc, int32 iPixelFormat, int32 iLayerPlane,
            uint32 nAttributes, const int32* piAttributes, int32* piValues);
    typedef bool (APIENTRY * FnWglChoosePixelFormatARB)(HDC hdc, const int32* piAttribList, const float* pfAttribFList,
        uint32 nMaxFormats, int32* piFormats, UINT *nNumFormats);
    typedef HGLRC (APIENTRY * FnWglCreateContextAttribsARB)(HDC, HGLRC, const int32*);
    typedef BOOL (APIENTRY * FnWglSwapInternalExt)(int32 interval);
    typedef int32 (APIENTRY * FnWglGetSwapInternalExt)();
    typedef const char* (APIENTRY * FnWglGetExtensionsStringARB)(HDC);

    WindowGLStaticContext();

    ~WindowGLStaticContext();

    NODISCARD bool support_extension() const
    {
        return wgl_get_pixel_format_attrib_ivarb_ && wgl_choose_pixel_format_arb_ && wgl_create_context_attribs_arb_;
    }

    NODISCARD String get_gl_string(uint32 which) const;
    NODISCARD String get_gl_error() const;

    FnWglCreateContext wgl_create_context_;
    FnWglDeleteContext wgl_delete_context_;
    FnWglGetCurrentContext wgl_get_current_context_;
    FnWglGetCurrentDC wgl_get_current_dc_;
    FnWglGetProcAddress wgl_get_proc_address_;
    FnWglMakeCurrent wgl_make_current_;
    FnWglShareLists wgl_share_lists_;

    FnWglGetPixelFormatAttribIVARB wgl_get_pixel_format_attrib_ivarb_;
    FnWglChoosePixelFormatARB wgl_choose_pixel_format_arb_;
    FnWglCreateContextAttribsARB wgl_create_context_attribs_arb_;
    FnWglSwapInternalExt wgl_swap_internal_ext_;
    FnWglGetSwapInternalExt wgl_get_swap_internal_ext_;
    FnWglGetExtensionsStringARB wgl_get_extensions_string_arb_;

    // @note do not call wgl version below directly as this might brake internalstate of opengl32.dll.
    // FnWglChoosePixelFormat wgl_choose_pixel_format_;
    // FnWglSetPixelFormat wgl_set_pixel_format_;
    // FnWglDescribePixelFormat wgl_describe_pixel_format_;
    // FnWglSwapBuffers wgl_swap_buffers_;

    void* resolve_symbol(StringView symbol) const
    {
        return dll_handle_ ? PlatformTraits::get_exported_symbol(dll_handle_, String(symbol)) : nullptr;
    }
private:
    // GL1+GLES2 common
    FnGlGetError gl_get_error_;
    FnGlGetIntegerv gl_get_integerv_;
    FnGlGetString gl_get_string_;

    void* dll_handle_{ nullptr };
};

class WindowsGLContext : public PlatformGLContext
{
public:
    struct ContextInfo
    {
        HWND hwnd;
        HDC hdc;
        HGLRC ctx;
    };

    WindowsGLContext(WindowGLStaticContext& static_context, const SurfaceFormat& request_format, const WindowsOpenGLAdditionalFormat& additional = {});
    ~WindowsGLContext() override;

    NODISCARD bool is_sharing() const override
    {
        return false;
    }

    NODISCARD bool valid() const override
    {
        return !!render_context_;
    }

    bool make_current(ApplicationWindow& window) override;

    bool swap_buffers(ApplicationWindow& window) override;

    void done_current() override;

    NODISCARD void* get_proc_address(StringView fn_name) const override
    {
        void* address = static_context_->wgl_get_proc_address_(fn_name.data());
        if (address == nullptr)
        {
            address = static_context_->resolve_symbol(fn_name.data());
        }
        CLOG_WARN(address == nullptr, rhi, "Failed to get proc address of {0}", fn_name);
        return address;
    }

private:
    ContextInfo* find_context(HWND hwnd);

    WindowGLStaticContext* static_context_;
    HGLRC render_context_{ nullptr };
    int32 pixel_format_{ 0 };
    PIXELFORMATDESCRIPTOR obtained_pixel_format_descriptor_;
    Array<ContextInfo> window_contexts_;
};

using PlatformGLContextImpl = WindowsGLContext;

}// namespace atlas