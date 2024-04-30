// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "platform/windows/windows_minimal_api.hpp"
// place this header after the windows header.
#include "gl/gl.h"

#include "platform/windows/windows_platform_traits.hpp"
#include "platform_gl_context.hpp"

namespace atlas
{

class WindowsOpenGL32
{
public:
    WindowsOpenGL32();
    ~WindowsOpenGL32();

    NODISCARD bool IsValid() const
    {
        return !!dll_handle_;
    }

    // WGL
    HGLRC (WINAPI * WglCreateContext)(HDC dc);
    BOOL (WINAPI * WglDeleteContext)(HGLRC context);
    HGLRC (WINAPI * WglGetCurrentContext)();
    HDC (WINAPI * WglGetCurrentDC)();
    PROC (WINAPI * WglGetProcAddress)(LPCSTR name);
    BOOL (WINAPI * WglMakeCurrent)(HDC dc, HGLRC context);
    BOOL (WINAPI * WglShareLists)(HGLRC context1, HGLRC context2);

    // GL1+GLES2 common
    GLenum (APIENTRY * GlGetError)();
    void (APIENTRY * GlGetIntegerv)(GLenum pname, GLint* params);
    const GLubyte * (APIENTRY * GlGetString)(GLenum name);

    void* ResolveGLSymbol(StringView symbol) const
    {
        return dll_handle_ ? PlatformTraits::GetExportedSymbol(dll_handle_, symbol) : nullptr;
    }
private:
    // For Mesa llvmpipe shipped with a name other than opengl32.dll
    BOOL (WINAPI * WglSwapBuffers)(HDC dc);
    BOOL (WINAPI * WglSetPixelFormat)(HDC dc, int pf, const PIXELFORMATDESCRIPTOR *pfd);
    int (WINAPI * WglDescribePixelFormat)(HDC dc, int pf, UINT size, PIXELFORMATDESCRIPTOR *pfd);

    void* dll_handle_{ nullptr };
};

class WindowGLStaticContext
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
    typedef BOOL (WINAPI * FnWglSetPixelFormat)(HDC dc, int pf, const PIXELFORMATDESCRIPTOR *pfd);
    typedef int (WINAPI * FnWglDescribePixelFormat)(HDC dc, int pf, UINT size, PIXELFORMATDESCRIPTOR *pfd);

    typedef GLenum (APIENTRY * FnGlGetError)();
    typedef void (APIENTRY * FnGlGetIntegerv)(GLenum pname, GLint* params);
    typedef const GLubyte * (APIENTRY * FnGlGetString)(GLenum name);

    typedef bool (APIENTRY * FnWglGetPixelFormatAttribIVARB)(HDC hdc, int32 iPixelFormat, int32 iLayerPlane,
            uint32 nAttributes, const int32* piAttributes, int32* piValues);
    typedef bool (APIENTRY * FnWglChoosePixelFormatARB)(HDC hdc, const int32* piAttribList, const float* pfAttribFList,
        uint32 nMaxFormats, int32* piFormats, UINT *nNumFormats);
    typedef HGLRC (APIENTRY * FnWglCreateContextAttribsARB)(HDC, HGLRC, const int32*);
    typedef BOOL (APIENTRY * FnWglSwapInternalExt)(int32 interval);
    typedef int32 (APIENTRY * FnWglGetSwapInternalExt)(void);
    typedef const char* (APIENTRY * FnWglGetExtensionsStringARB)(HDC);

    WindowGLStaticContext();

    ~WindowGLStaticContext();

    NODISCARD bool SupportExtension() const
    {
        return wgl_get_pixel_format_attrib_ivarb_ && wgl_choose_pixel_format_arb_ && wgl_create_context_attribs_arb_;
    }

    NODISCARD String GetGLString(uint32 which) const;
    NODISCARD String GetGLError() const;

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
    FnWglGetExtensionsStringARB wgl_get_extensions_string_arb_;
private:
    // GL1+GLES2 common
    FnGlGetError gl_get_error_;
    FnGlGetIntegerv gl_get_integerv_;
    FnGlGetString gl_get_string_;
    // For Mesa llvmpipe shipped with a name other than opengl32.dll
    FnWglSwapBuffers wgl_swap_buffers_;
    FnWglSetPixelFormat wgl_set_pixel_format_;
    FnWglDescribePixelFormat wgl_describe_pixel_format_;

    void* ResolveSymbol(StringView symbol) const
    {
        return dll_handle_ ? PlatformTraits::GetExportedSymbol(dll_handle_, symbol) : nullptr;
    }

    void* dll_handle_{ nullptr };
};

class WindowsGLContext : public PlatformGLContext
{
    struct ContextInfo
    {
        HWND hwnd;
        HDC hdc;
        HGLRC ctx;
    };

public:
    WindowsGLContext();
    ~WindowsGLContext() override;

    bool MakeCurrent(const ApplicationWindow& window) override;

private:
    static inline WindowGLStaticContext static_context_;

    HGLRC render_context_{ nullptr };
    int32 pixel_format_{ 0 };
    PIXELFORMATDESCRIPTOR obtained_pixel_format_descriptor_;
    Array<ContextInfo> window_contexts_;
};

}// namespace atlas