// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "application_window.hpp"
#include "platform/windows/windows_minimal_api.hpp"

namespace atlas
{

class WindowsApplication;

class WindowsWindow : public ApplicationWindow, public std::enable_shared_from_this<WindowsWindow>
{
    using base = ApplicationWindow;
    struct Private {};
public:
    static std::shared_ptr<WindowsWindow> create(const WindowsApplication& application, const WindowDescription& description, const ApplicationWindow* parent = nullptr);

    explicit WindowsWindow(Private) {};

    ~WindowsWindow() override
    {
        deinitialize();
    }

    void destroy() override;

    void show() override;

    void hide() override;

    void* get_native_handle() const override
    {
        return window_handle_;
    }

    NODISCARD HWND native_handle() const
    {
        return window_handle_;
    }

protected:
    void initialize(const WindowsApplication& application, const WindowDescription& description, const ApplicationWindow* parent);
    void deinitialize();

public:
    static inline const wchar_t* window_class_name_ = L"Atlas";

protected:
    HWND window_handle_{ nullptr };
    bool is_visible_{ false };
};

}// namespace atlas