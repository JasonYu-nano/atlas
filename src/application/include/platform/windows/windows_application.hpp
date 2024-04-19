// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "application_implement.hpp"
#include "platform/windows/windows_minimal_api.hpp"

namespace atlas
{

class APPLICATION_API WindowsApplication : public ApplicationImplement
{
public:
    void Initialize() override;
    void Deinitialize() override;
    void Tick(float delta_time) override;
private:
    void RegisterWinClass();
    void ShowWindow();
    static LRESULT CALLBACK HandleWindowsMsg(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

    HINSTANCE hinstance_{ nullptr };
    HWND hwnd_{ nullptr };
    HDC hdc_{ nullptr };
};

} // namespace atlas