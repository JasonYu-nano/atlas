// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "platform/windows/windows_window.hpp"
#include "platform/windows/windows_application.hpp"
#include "application_log.hpp"

namespace atlas
{

std::shared_ptr<WindowsWindow> WindowsWindow::Create(const WindowsApplication& application, const WindowDescription& description, std::shared_ptr<ApplicationWindow> parent)
{
    auto window = std::make_shared<WindowsWindow>(Private{});
    window->Initialize(application, description, parent);
    return window;
}

void WindowsWindow::Destroy()
{
    if (initialized_)
    {
        Deinitialize();
        on_window_destroyed_.Broadcast(shared_from_this());
    }
}

void WindowsWindow::Initialize(const WindowsApplication& application, const WindowDescription& description, std::shared_ptr<ApplicationWindow> parent)
{
    uint32 window_style = 0;
    uint32 window_ex_style = 0;
    int32 additional_width = 0;
    int32 additional_height = 0;

    if (description.is_frameless)
    {

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
        additional_height += ::GetSystemMetrics(SM_CYCAPTION);
    }

    if (description.is_frameless)
    {
        additional_width += ::GetSystemMetrics(SM_CXSIZEFRAME);
        additional_height += ::GetSystemMetrics(SM_CYSIZEFRAME);
    }

    window_handle_ = CreateWindowEx(
        window_ex_style,
        window_class_name_,
        description.title.ToWide().data(),
        window_style,
        description.x,
        description.y,
        description.width + additional_width,
        description.height + additional_height,
        parent ? static_cast<HWND>(parent->GetNativeHandle()) : nullptr,
        NULL,
        application.GetInstanceHandle(),
        NULL);

    if (!window_handle_)
    {
        LOG_ERROR(app, "CreateWindowEx failed, error code: {0}", ::GetLastError());
        return;
    }

    device_context_handle = GetDC(window_handle_);

    // display the window on the screen
    ::ShowWindow(window_handle_, SW_SHOW);

    initialized_ = true;
}

void WindowsWindow::Deinitialize()
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