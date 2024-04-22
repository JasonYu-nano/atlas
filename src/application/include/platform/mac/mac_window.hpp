// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <memory>

#include "application_window.hpp"
#include "core_macro.hpp"


FORWARD_DECLARE_OBJC_CLASS(NSWindow);

namespace atlas
{

class MacApplication;

/**
 * @brief Wrapper class for cocoa window.
 * Use MacWindow::Create to create instance.
 */
class MacWindow : public ApplicationWindow, public std::enable_shared_from_this<MacWindow>
{
    using base = ApplicationWindow;
    struct Private {};
public:
    /**
     * @brief Create a native window.
     * @param application
     * @return
     */
    static std::shared_ptr<MacWindow> Create(const MacApplication& application, const WindowDescription& description,
                                             const MacWindow* parent = nullptr);

    // make sure MacWindow can only be construct by MacWindow::Create()
    explicit MacWindow(Private) {};

    ~MacWindow() override
    {
        MacWindow::Destroy();
    }

    void Destroy() override;

    NODISCARD bool CanBecomePrimary() const override
    {
        return can_be_primary_;
    }

    NODISCARD void* GetNativeHandle() const override
    {
        return native_window_;
    }

protected:
    void Initialize(const MacApplication& application, const WindowDescription& description, const MacWindow* parent);
    void Deinitialize();

    bool can_be_primary_{ true };
    NSWindow* native_window_{ nullptr };
};

} // namespace atlas