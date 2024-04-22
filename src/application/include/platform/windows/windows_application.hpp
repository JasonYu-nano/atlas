// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "application_implement.hpp"
#include "platform/windows/windows_minimal_api.hpp"

namespace atlas
{

class WindowsWindow;

class APPLICATION_API WindowsApplication : public ApplicationImplement
{
public:
    WindowsApplication() : hinstance_(GetModuleHandle(nullptr)) {}

    void Initialize() override;
    void Deinitialize() override;
    void Tick(float delta_time) override;

    std::shared_ptr<ApplicationWindow> MakeWindow(const WindowDescription& description) override;

    NODISCARD HINSTANCE GetInstanceHandle() const
    {
        return hinstance_;
    }

    NODISCARD std::shared_ptr<ApplicationWindow> GetWindow(HWND hwnd) const;

private:
    void RegisterWindowClass();

    void OnWindowDestroyed(std::shared_ptr<ApplicationWindow> window);

    static LRESULT CALLBACK HandleWindowsMsg(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

    LRESULT ProcessMessage(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

    DelegateHandle windows_destroy_handle_;
    HINSTANCE hinstance_{ nullptr };
    std::weak_ptr<ApplicationWindow> primary_window_;
    Array<std::shared_ptr<ApplicationWindow>> managed_windows_;
};

} // namespace atlas