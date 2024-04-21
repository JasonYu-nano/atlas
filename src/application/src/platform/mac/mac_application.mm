// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#import "Cocoa/Cocoa.h"

#include <ranges>

#include "platform/mac/mac_application.hpp"
#include "platform/mac/mac_window.hpp"
#include "engine.hpp"

namespace atlas
{

void MacApplication::Initialize()
{
    windows_destroy_handle_ = MacWindow::on_window_destroyed_.AddRaw(this, &MacApplication::OnWindowDestroyed);
}

void MacApplication::Deinitialize()
{
    if (windows_destroy_handle_.IsValid())
    {
        MacWindow::on_window_destroyed_.Remove(windows_destroy_handle_);
    }

    for (auto&& windows : managed_windows)
    {
        windows->Destroy();
    }
    managed_windows.Clear();
}

void MacApplication::Tick(float delta_time)
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

std::shared_ptr<ApplicationWindow> MacApplication::MakeWindow()
{
    auto&& window = MacWindow::Create(this);
    managed_windows.Add(window);

    if (window->CanBecomePrimary() && primary_window_.expired())
    {
        primary_window_ = window;
        [NSApp activateIgnoringOtherApps:YES];
    }

    return window;
}

void MacApplication::OnWindowDestroyed(std::shared_ptr<ApplicationWindow> window)
{
    if (!primary_window_.expired())
    {
        auto shared_primary_window = primary_window_.lock();
        if (shared_primary_window == window)
        {
            primary_window_.reset();
        }
    }

    managed_windows.Remove(window);

    bool need_shutdown_engine = false;
    if (primary_window_.expired())
    {
        // find next primary window.
        for (auto&& managed_window : std::ranges::reverse_view(managed_windows))
        {
            if (managed_window->CanBecomePrimary())
            {
                primary_window_ = managed_window;
                break;
            }
        }

        need_shutdown_engine = primary_window_.expired();
    }

    if (need_shutdown_engine)
    {
        g_engine->RequestShutdown();
    }
}

} // namespace atlas