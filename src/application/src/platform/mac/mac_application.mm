// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#import "Cocoa/Cocoa.h"

#include <ranges>

#include "platform/mac/mac_application.hpp"
#include "platform/mac/mac_window.hpp"
#include "engine.hpp"

namespace atlas
{

void MacApplication::initialize()
{
    windows_destroy_handle_ = MacWindow::on_window_destroyed_.add_raw(this, &MacApplication::on_window_destroyed);
}

void MacApplication::deinitialize()
{
    if (windows_destroy_handle_.is_valid())
    {
        MacWindow::on_window_destroyed_.remove(windows_destroy_handle_);
    }

    for (auto&& windows : managed_windows)
    {
        windows->destroy();
    }
    managed_windows.clear();
}

void MacApplication::tick(float delta_time)
{
    while (NSEvent *Event = [NSApp nextEventMatchingMask: NSEventMaskAny untilDate: nil inMode: NSDefaultRunLoopMode dequeue: true])
    {
        // Either the windowNumber is 0 or the window must be valid for the event to be processed.
        // Processing events with a windowNumber but no window will crash inside sendEvent as it will try to send to a destructed window.
        if ([Event windowNumber] == 0 || [Event window] != nil)
        {
            [NSApp sendEvent: Event];
        }
    }
}

std::shared_ptr<ApplicationWindow> MacApplication::make_window(const WindowDescription& description, const ApplicationWindow* parent)
{
    auto&& window = MacWindow::create(*this, description, static_cast<const MacWindow*>(parent));
    managed_windows.add(window);

    if (window->can_become_primary() && primary_window_.expired())
    {
        primary_window_ = window;
        [NSApp activateIgnoringOtherApps:YES];
    }

    return window;
}

void MacApplication::on_window_destroyed(std::shared_ptr<ApplicationWindow> window)
{
    if (!primary_window_.expired())
    {
        auto shared_primary_window = primary_window_.lock();
        if (shared_primary_window == window)
        {
            primary_window_.reset();
        }
    }

    managed_windows.remove(window);

    bool need_shutdown_engine = false;
    if (primary_window_.expired())
    {
        // find the next primary window.
        for (auto&& managed_window : std::ranges::reverse_view(managed_windows))
        {
            if (managed_window->can_become_primary())
            {
                primary_window_ = managed_window;
                break;
            }
        }

        need_shutdown_engine = primary_window_.expired();
    }

    if (need_shutdown_engine)
    {
        g_engine->request_shutdown();
    }
}

} // namespace atlas