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

    void initialize() override;
    void deinitialize() override;
    void tick(float delta_time) override;

    std::shared_ptr<ApplicationWindow> make_window(const WindowDescription& description, const ApplicationWindow* parent) override;

    std::shared_ptr<ApplicationWindow> make_dummy_window() override;

    std::shared_ptr<ApplicationWindow> get_key_window() const override;

    NODISCARD HINSTANCE get_instance_handle() const
    {
        return hinstance_;
    }

    NODISCARD std::shared_ptr<ApplicationWindow> GetWindow(HWND hwnd) const;

private:
    void register_window_class();

    void on_window_destroyed(std::shared_ptr<ApplicationWindow> window);

    static LRESULT CALLBACK handle_windows_msg(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

    LRESULT process_message(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

    DelegateHandle windows_destroy_handle_;
    HINSTANCE hinstance_{ nullptr };
    std::weak_ptr<ApplicationWindow> primary_window_;
    Array<std::shared_ptr<ApplicationWindow>> managed_windows_;
};

} // namespace atlas