// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "core_def.hpp"
#include "misc/delegate_fwd.hpp"

namespace atlas
{

class ApplicationImplement;

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
    virtual void Destroy()
    {
        Deinitialize();
    }
    /**
     * @brief Determines whether the window can become the main window.
     * @return
     */
    NODISCARD virtual bool CanBecomePrimary() const
    {
        return true;
    }

    DECLARE_MULTICAST_DELEGATE_ONE_PARAM(OnWindowDestroyed, std::shared_ptr<ApplicationWindow>, window);
    /**
     * @brief Event broadcast when a window was destroyed.
     */
    static inline OnWindowDestroyed on_window_destroyed_;

protected:
    virtual void Initialize(ApplicationImplement* application)
    {
        initialized_ = true;
    }

    virtual void Deinitialize()
    {
        initialized_ = false;
    }

    bool initialized_{ false };
};

}// namespace atlas