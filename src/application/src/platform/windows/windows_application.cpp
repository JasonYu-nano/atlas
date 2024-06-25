// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "platform/windows/windows_application.hpp"

#include "engine.hpp"
#include "application_log.hpp"
#include "platform/windows/windows_window.hpp"

namespace atlas
{

static WindowsApplication* g_application = nullptr;

void WindowsApplication::initialize()
{
    windows_destroy_handle_ = WindowsWindow::on_window_destroyed_.add_raw(this, &WindowsApplication::on_window_destroyed);
    register_window_class();
    g_application = this;
}

void WindowsApplication::deinitialize()
{
    if (windows_destroy_handle_.is_valid())
    {
        WindowsWindow::on_window_destroyed_.remove(windows_destroy_handle_);
        windows_destroy_handle_.invalidate();
    }

    if (g_application == this)
    {
        g_application = nullptr;
    }
}

void WindowsApplication::tick(float delta_time)
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

std::shared_ptr<ApplicationWindow> WindowsApplication::make_window(const WindowDescription& description, const ApplicationWindow* parent)
{
    auto&& window = WindowsWindow::create(*this, description, parent);
    managed_windows_.add(window);

    if (primary_window_.expired() && window->can_become_primary())
    {
        primary_window_ = window;
    }

    return window;
}

std::shared_ptr<ApplicationWindow> WindowsApplication::make_dummy_window()
{
    // we won't manage dummy window.
    WindowDescription desc;
    desc.display_when_initialize = false;
    return WindowsWindow::create(*this, desc, nullptr);
}

std::shared_ptr<ApplicationWindow> WindowsApplication::get_key_window() const
{
    return primary_window_.expired() ? nullptr : primary_window_.lock();
}

std::shared_ptr<ApplicationWindow> WindowsApplication::get_window(HWND hwnd) const
{
    const size_t index = managed_windows_.find([hwnd](auto&& window) {
        return window->native_handle() == hwnd;
    });
    return index == INDEX_NONE ? nullptr : managed_windows_[index];
}

void WindowsApplication::register_window_class()
{
    // this struct holds information for the window class
    WNDCLASSEX wc;

    // clear out the window class for use
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    // fill in the struct with the needed information
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = handle_windows_msg;
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
            g_engine->request_shutdown();
        }
    }
}

void WindowsApplication::on_window_destroyed(std::shared_ptr<ApplicationWindow> window)
{
    if (!primary_window_.expired())
    {
        auto&& shared_primary_window = primary_window_.lock();
        if (shared_primary_window == window)
        {
            primary_window_.reset();
        }
    }

    managed_windows_.remove(std::static_pointer_cast<WindowsWindow>(window));

    bool need_shutdown_engine = managed_windows_.is_empty();
    if (!need_shutdown_engine && primary_window_.expired())
    {
        // find next primary window.
        for (auto&& managed_window : managed_windows_)
        {
            if (managed_window->can_become_primary())
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

LRESULT WindowsApplication::handle_windows_msg(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    if (g_application)
    {
        return g_application->process_message(hwnd, message, wparam, lparam);
    }
    else
    {
        return ::DefWindowProc(hwnd, message, wparam, lparam);
    }
}

LRESULT WindowsApplication::process_message(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    auto&& window = get_window(hwnd);
    if (!window)
    {
        return ::DefWindowProc(hwnd, message, wparam, lparam);
    }

    LRESULT result = 0;

    switch (message)
    {
        case WM_DESTROY:
        {
            window->destroy();
        } break;
        default:
            result = ::DefWindowProc(hwnd, message, wparam, lparam);
    }

    return result;
}

}// namespace atlas

