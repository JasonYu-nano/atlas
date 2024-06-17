// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "platform/windows/windows_application.hpp"

#include "engine.hpp"
#include "log/logger.hpp"

namespace atlas
{

void WindowsApplication::Initialize()
{
    RegisterWinClass();
    ShowWindow();
}

void WindowsApplication::DeInitialize()
{

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

void WindowsApplication::RegisterWinClass()
{
    hinstance_ = GetModuleHandle(nullptr);

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
    wc.lpszClassName = L"ATLAS"; //TODO:

    // register the window class
    auto result = RegisterClassEx(&wc);
    if (result == 0)
    {
        LOG_ERROR(temp, "RegisterClassEx failed in windows platform");
        if (g_engine)
        {
            g_engine->request_shutdown();
        }
    }
}

void WindowsApplication::ShowWindow()
{
    int heightAdjust = (GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CXPADDEDBORDER));
    int widthAdjust = (GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER));


    // create the window and use the result as the handle
    // TODO: move args to window definition
    hwnd_ = CreateWindowEx(
        0,
        L"ATLAS",                               // name of the window class TODO:
        L"Title",                                // title of the window TODO:
        WS_OVERLAPPEDWINDOW,                        // window style
        CW_USEDEFAULT,                              // x-position of the window
        CW_USEDEFAULT,                              // y-position of the window
        800 + widthAdjust,           // width of the window
        600 + heightAdjust,         // height of the window
        NULL,                                       // we have no parent window, NULL
        NULL,                                       // we aren't using menus, NULL
        hinstance_,                                   // application handle
        NULL);                                      // pass pointer to current object

    if (hwnd_ == nullptr)
    {
        uint32 errorCode = (uint32)::GetLastError();
        LOG_ERROR(temp, "CreateWindowEx failed, error code: {0}", errorCode);
        return;
    }

    hdc_ = GetDC(hwnd_);

    // display the window on the screen
    ::ShowWindow(hwnd_, SW_SHOW);
}

LRESULT WindowsApplication::HandleWindowsMsg(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    LRESULT result = 0;

    switch (message)
    {
        case WM_DESTROY:
        {
            if (g_engine)
            {
                g_engine->request_shutdown();
            }
            PostQuitMessage(0);
        } break;
        default:
            result = ::DefWindowProc(hwnd, message, wparam, lparam);
    }

    return result;
}

}// namespace atlas

