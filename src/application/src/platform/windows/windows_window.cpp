// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "platform/windows/windows_window.hpp"
#include "platform/windows/windows_application.hpp"
#include "application_log.hpp"

namespace atlas
{

std::shared_ptr<WindowsWindow> WindowsWindow::Create(WindowsApplication* application)
{
    auto window = std::make_shared<WindowsWindow>(Private{});
    window->Initialize(application);
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

void WindowsWindow::Initialize(ApplicationImplement* application)
{
    base::Initialize(application);

    int heightAdjust = (GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CXPADDEDBORDER));
    int widthAdjust = (GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER));

    WindowsApplication* win_application = static_cast<WindowsApplication*>(application);
    window_handle_ = CreateWindowEx(
        0,
        window_class_name_,                               // name of the window class
        L"Title1",                                // title of the window TODO:
        WS_OVERLAPPEDWINDOW,                        // window style
        CW_USEDEFAULT,                              // x-position of the window
        CW_USEDEFAULT,                              // y-position of the window
        800 + heightAdjust,           // width of the window
        600 + widthAdjust,         // height of the window
        NULL,                                       // we have no parent window, NULL
        NULL,                                       // we aren't using menus, NULL
        win_application->GetInstanceHandle(),                                   // application handle
        NULL);                                      // pass pointer to current object

    if (!window_handle_)
    {
        LOG_ERROR(app, "CreateWindowEx failed, error code: {0}", ::GetLastError());
        return;
    }

    device_context_handle = GetDC(window_handle_);

    // display the window on the screen
    ::ShowWindow(window_handle_, SW_SHOW);
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
        base::Deinitialize();
    }
}

}// namespace atlas