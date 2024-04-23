// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "core_def.hpp"
#include "misc/delegate_fwd.hpp"
#include "string/string.hpp"
#include "math/rect.hpp"

namespace atlas
{

class ApplicationImplement;

using FrameRect = Rect<int32>;

/**
 * @brief The definitions of native window property.
 */
struct APPLICATION_API WindowDescription
{
    /** Title of the window */
    String title = "";
    /** Whether the window is frameless */
    bool is_frameless = false;
    /** Whether the window is start with fullscreen */
    bool is_fullscreen = true;
    /** Whether the window has a close button. Only works when is_frameless == false && is_fullscreen == false */
    bool is_closeable = true;
    /** Whether the window can resize */
    bool is_resizeable = true;
    /** Whether the window can be miniaturize to the taskbar. Only works when is_frameless == false && is_fullscreen == false */
    bool is_miniaturizable = true;
    /** Whether the window is start with max size */
    bool is_maximize = false;
    /** Whether the window is start with min size */
    bool is_minimize = false;
    /** The content frame rect of the window */
    FrameRect frame_rect = {100, 100, 800, 600};
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