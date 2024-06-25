// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "platform/windows/windows_window.hpp"
#include "platform/windows/windows_application.hpp"
#include "application_log.hpp"

namespace atlas
{

std::shared_ptr<WindowsWindow> WindowsWindow::create(const WindowsApplication& application, const WindowDescription& description, const ApplicationWindow* parent)
{
    auto window = std::make_shared<WindowsWindow>(Private{});
    window->initialize(application, description, parent);
    return window;
}

void WindowsWindow::destroy()
{
    if (initialized_)
    {
        deinitialize();
        on_window_destroyed_.broadcast(shared_from_this());
    }
}

void WindowsWindow::show()
{
    if (!is_visible_)
    {
        is_visible_ = true;
        ::ShowWindow(window_handle_, SW_SHOW);
    }
}

void WindowsWindow::hide()
{
    if (is_visible_)
    {
        is_visible_ = false;
        ::ShowWindow(window_handle_, SW_HIDE);
    }
}

void WindowsWindow::initialize(const WindowsApplication& application, const WindowDescription& description, const ApplicationWindow* parent)
{
    uint32 window_style = 0;
    uint32 window_ex_style = 0;

    int32 x = description.frame_rect.x;
    int32 y = description.frame_rect.y;
    int32 width = description.frame_rect.width;
    int32 height = description.frame_rect.height;

    if (description.is_frameless)
    {
        width += ::GetSystemMetrics(SM_CXSIZEFRAME);
        height += ::GetSystemMetrics(SM_CYSIZEFRAME);
    }
    else if (description.is_fullscreen)
    {
        window_style = WS_POPUP | WS_VISIBLE;
        x = y = 0;
        width = ::GetSystemMetrics(SM_CXSCREEN);
        height = ::GetSystemMetrics(SM_CYSCREEN);
    }
    else
    {
        if (description.is_closeable)
        {
            window_style |= WS_CAPTION|WS_SYSMENU;
        }

        if (description.is_miniaturizable)
        {
            window_style |= WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX;
        }

        if (description.is_maximize)
        {
            window_style |= WS_MAXIMIZE;
        }
        else if (description.is_minimize)
        {
            window_style |= WS_MINIMIZE;
        }

        if (window_style & (WS_SYSMENU|WS_CAPTION))
        {
            height += ::GetSystemMetrics(SM_CYCAPTION);
        }
    }

    window_handle_ = CreateWindowEx(
        window_ex_style,
        window_class_name_,
        description.title.to_wide().data(),
        window_style,
        x,
        y,
        width,
        height,
        parent ? static_cast<HWND>(parent->native_handle()) : nullptr,
        NULL,
        application.get_instance_handle(),
        NULL);

    if (!window_handle_)
    {
        LOG_ERROR(app, "CreateWindowEx failed, error code: {0}", ::GetLastError());
        return;
    }

    if (description.display_when_initialize)
    {
        show();
    }

    initialized_ = true;
}

void WindowsWindow::deinitialize()
{
    if (initialized_)
    {
        if (window_handle_)
        {
            HWND handle = std::exchange(window_handle_, nullptr);
            DestroyWindow(handle);
        }
        initialized_ = false;
    }
}

}// namespace atlas