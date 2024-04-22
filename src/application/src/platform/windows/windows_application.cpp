// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "platform/windows/windows_application.hpp"

#include "engine.hpp"
#include "application_log.hpp"
#include "platform/windows/windows_window.hpp"

namespace atlas
{

static WindowsApplication* g_application = nullptr;

void WindowsApplication::Initialize()
{
    windows_destroy_handle_ = WindowsWindow::on_window_destroyed_.AddRaw(this, &WindowsApplication::OnWindowDestroyed);
    RegisterWindowClass();
    g_application = this;
}

void WindowsApplication::Deinitialize()
{
    if (windows_destroy_handle_.IsValid())
    {
        WindowsWindow::on_window_destroyed_.Remove(windows_destroy_handle_);
        windows_destroy_handle_.Invalidate();
    }

    if (g_application == this)
    {
        g_application = nullptr;
    }
}

void WindowsApplication::Tick(float delta_time)
{
    MSG msg;
    // we use PeekMessage instead of GetMessage here
    // because we should not block the thread at anywhere
    // except the engine execution driver module
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        // translate keystroke messages into the right format
        TranslateMessage(&msg);

        // send the message to the WindowProc function
        DispatchMessage(&msg);
    }
}

std::shared_ptr<ApplicationWindow> WindowsApplication::MakeWindow()
{
    auto&& window = WindowsWindow::Create(this);
    managed_windows_.Add(window);

    if (primary_window_.expired() && window->CanBecomePrimary())
    {
        primary_window_ = window;
    }

    return window;
}

std::shared_ptr<ApplicationWindow> WindowsApplication::GetWindow(HWND hwnd) const
{
    const size_t index = managed_windows_.Find([hwnd](auto&& window) {
        return window->GetNativeHandle() == hwnd;
    });
    return index == INDEX_NONE ? nullptr : managed_windows_[index];
}

void WindowsApplication::RegisterWindowClass()
{
    // this struct holds information for the window class
    WNDCLASSEX wc;

    // clear out the window class for use
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    // fill in the struct with the needed information
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = HandleWindowsMsg;
    wc.hInstance = hinstance_;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = WindowsWindow::window_class_name_;

    // register the window class
    ATOM result = RegisterClassEx(&wc);
    if (result == 0)
    {
        LOG_ERROR(app, "RegisterClassEx failed in windows platform");
        if (g_engine)
        {
            g_engine->RequestShutdown();
        }
    }
}

void WindowsApplication::OnWindowDestroyed(std::shared_ptr<ApplicationWindow> window)
{
    if (!primary_window_.expired())
    {
        auto&& shared_primary_window = primary_window_.lock();
        if (shared_primary_window == window)
        {
            primary_window_.reset();
        }
    }

    managed_windows_.Remove(std::static_pointer_cast<WindowsWindow>(window));

    bool need_shutdown_engine = managed_windows_.IsEmpty();
    if (!need_shutdown_engine && primary_window_.expired())
    {
        // find next primary window.
        for (auto&& managed_window : managed_windows_)
        {
            if (managed_window->CanBecomePrimary())
            {
                primary_window_ = managed_window;
                break;
            }
        }

        need_shutdown_engine = primary_window_.expired();
    }

    if (need_shutdown_engine)
    {
        if (g_engine)
        {
            g_engine->request_shutdown();
        }
    }
}

LRESULT WindowsApplication::HandleWindowsMsg(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    if (g_application)
    {
        return g_application->ProcessMessage(hwnd, message, wparam, lparam);
    }
    else
    {
        return ::DefWindowProc(hwnd, message, wparam, lparam);
    }
}

LRESULT WindowsApplication::ProcessMessage(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    auto&& window = GetWindow(hwnd);
    if (!window)
    {
        return ::DefWindowProc(hwnd, message, wparam, lparam);
    }

    LRESULT result = 0;

    switch (message)
    {
        case WM_DESTROY:
        {
            window->Destroy();
        } break;
        default:
            result = ::DefWindowProc(hwnd, message, wparam, lparam);
    }

    return result;
}

}// namespace atlas

