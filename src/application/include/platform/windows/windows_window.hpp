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
    static std::shared_ptr<WindowsWindow> Create(WindowsApplication* application);

    explicit WindowsWindow(Private) {};

    void Destroy() override;

    void* GetNativeHandle() const override
    {
        return window_handle_;
    }

    NODISCARD HWND GetHandle() const
    {
        return window_handle_;
    }

protected:
    void Initialize(ApplicationImplement* application) override;
    void Deinitialize() override;

public:
    static inline const wchar_t* window_class_name_ = L"Atlas";

protected:
    HWND window_handle_{ nullptr };
    HDC device_context_handle{ nullptr };
};

}// namespace atlas