// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "core_def.hpp"
#include "misc/delegate_fwd.hpp"
#include "string/string.hpp"

namespace atlas
{

class ApplicationImplement;

struct APPLICATION_API WindowDescription
{
    String title = "";
    bool is_closeable = false;
    bool is_resizeable = true;
    bool is_miniaturizable = true;
    bool is_frameless = false;
    bool is_maximize = false;
    bool is_minimize = false;
    int32 x = 100;
    int32 y = 100;
    int32 width = 800;
    int32 height = 600;
};

/**
 * @brief The base class of native window.
 */
class APPLICATION_API ApplicationWindow
{
public:
    virtual ~ApplicationWindow() = default;

    /**
     * @brief Manually destroy the window. Can be called repeatedly.
     * If the window is a primary window, Destroy() may cause engine shutdown.
     */
    virtual void Destroy() {}
    /**
     * @brief Determines whether the window can become the main window.
     * @return
     */
    NODISCARD virtual bool CanBecomePrimary() const
    {
        return true;
    }

    /**
     * @brief Get handle of native window.
     * eg: NSWindow, HWND...
     * @return
     */
    NODISCARD virtual void* GetNativeHandle() const
    {
        return nullptr;
    }
    DECLARE_MULTICAST_DELEGATE_ONE_PARAM(OnWindowDestroyed, std::shared_ptr<ApplicationWindow>, window);
    /**
     * @brief Event broadcast when a window was destroyed.
     */
    static inline OnWindowDestroyed on_window_destroyed_;

protected:

    bool initialized_{ false };
};

}// namespace atlas