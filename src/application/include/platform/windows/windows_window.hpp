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
    static std::shared_ptr<WindowsWindow> Create(const WindowsApplication& application, const WindowDescription& description, const ApplicationWindow* parent = nullptr);

    explicit WindowsWindow(Private) {};

    ~WindowsWindow() override
    {
        Deinitialize();
    }

    void Destroy() override;

    void Show() override;

    void Hide() override;

    void* get_native_handle() const override
    {
        return window_handle_;
    }

    NODISCARD HWND GetHandle() const
    {
        return window_handle_;
    }

protected:
    void Initialize(const WindowsApplication& application, const WindowDescription& description, const ApplicationWindow* parent);
    void Deinitialize();

public:
    static inline const wchar_t* window_class_name_ = L"Atlas";

protected:
    HWND window_handle_{ nullptr };
    bool is_visible_{ false };
};

}// namespace atlas