// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#import "platform/mac/mac_window.hpp"
#import "Cocoa/Cocoa.h"

@interface WindowDelegate : NSObject<NSWindowDelegate>
- (instancetype)init:(atlas::MacWindow*) window;
@property atlas::MacWindow* associated_window;
@end

@implementation WindowDelegate

- (instancetype)init:(atlas::MacWindow *)window
{
    self = [super init];
    if (self)
    {
        _associated_window = window;
    }
    return self;
}

- (BOOL)windowShouldClose:(NSWindow *)sender
{
    // 处理窗口关闭事件，返回YES表示允许关闭窗口，返回NO表示阻止关闭窗口
    return YES;
}

- (void)windowWillClose:(NSNotification *)notification
{
    // triggers when user click close button. let mac windows know about close event.
    if (_associated_window)
    {
        _associated_window->Destroy();
    }
}

@end

namespace atlas
{

std::shared_ptr<MacWindow> MacWindow::Create(ApplicationImplement* application)
{
    auto window = std::make_shared<MacWindow>(Private{});
    window->Initialize(application);
    return window;
}

void MacWindow::Initialize(ApplicationImplement* application)
{
    base::Initialize(application);

    NSRect frame = NSMakeRect(500, 500, 500, 500);
    native_window_ = [[NSWindow alloc] initWithContentRect:frame
                                                 styleMask:NSWindowStyleMaskTitled|NSWindowStyleMaskClosable|NSWindowStyleMaskResizable|NSWindowStyleMaskMiniaturizable
                                                   backing:NSBackingStoreBuffered defer:NO];

    NSString *title = @"Title";
    [native_window_ setTitle:title];
    [native_window_ setBackgroundColor:[NSColor whiteColor]];
    [native_window_ makeKeyAndOrderFront:NSApp];
    [native_window_ setDelegate:[[WindowDelegate alloc] init:this]];
}

void MacWindow::Deinitialize()
{
    if (initialized_)
    {
        if (native_window_)
        {
            [native_window_ close];
            native_window_ = nullptr;
        }

        base::Deinitialize();
    }
}

void MacWindow::Destroy()
{
    if (initialized_)
    {
        if (native_window_)
        {
            [native_window_ setDelegate:nil]; // we don't need route event to window delegate
        }
        Deinitialize();
        on_window_destroyed_.Broadcast(shared_from_this());
    }
}

} // namespace atlas
