// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "core_def.hpp"
#include "platform/windows/windows_minimal_api.hpp"

namespace atlas
{

class APPLICATION_API WindowsApplication
{
public:
    void Initialize();
    void DeInitialize();
    void Tick(float delta_time);
private:
    void RegisterWinClass();
    void ShowWindow();
    static LRESULT CALLBACK HandleWindowsMsg(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

    HINSTANCE hinstance_{ nullptr };
    HWND hwnd_{ nullptr };
    HDC hdc_{ nullptr };
};

using PlatformApplication = WindowsApplication;

} // namespace atlas