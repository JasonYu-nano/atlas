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
        _associated_window->destroy();
    }
}

@end

namespace atlas
{

NSRect from_rect(const FrameRect& rect)
{
    NSScreen* main_screen = [NSScreen mainScreen];
    NSRect screen_frame = [main_screen frame];
    NSSize screen_size = screen_frame.size;
    CGFloat frame_width = screen_size.width;
    CGFloat frame_height = screen_size.height;

    NSRect frame = NSMakeRect(rect.x, frame_height - rect.y - rect.height, rect.width, rect.height);
    return frame;
}

std::shared_ptr<MacWindow> MacWindow::create(const MacApplication& application, const WindowDescription& description,
                                             const MacWindow* parent)
{
    auto window= std::make_shared<MacWindow>(Private{});
    window->initialize(application, description, parent);
    return window;
}

void MacWindow::initialize(const MacApplication& application, const WindowDescription& description, const MacWindow* parent)
{
    NSWindowStyleMask style = NSWindowStyleMaskBorderless;

    if (description.is_frameless)
    {

    }
    else
    {
        if (description.is_closeable)
        {
            style |= NSWindowStyleMaskTitled|NSWindowStyleMaskClosable;
        }

        if (description.is_miniaturizable)
        {
            style |= NSWindowStyleMaskTitled|NSWindowStyleMaskMiniaturizable;
        }

        if (description.is_resizeable)
        {
            style |= NSWindowStyleMaskTitled|NSWindowStyleMaskResizable;
        }
    }

    NSRect rect;
    if (description.is_maximize)
    {
        NSScreen* main_screen = [NSScreen mainScreen];
        NSRect screen_frame = [main_screen frame];
        NSSize screen_size = screen_frame.size;
        rect = NSMakeRect(0, 0, screen_size.width, screen_size.height);
    }
    else
    {
        rect = from_rect(description.frame_rect);
    }

    native_window_ = [[NSWindow alloc] initWithContentRect:rect styleMask:style backing:NSBackingStoreBuffered defer:NO];

    [native_window_ setTitle:[NSString stringWithUTF8String:description.title.data()]];
    [native_window_ setBackgroundColor:[NSColor whiteColor]];
    [native_window_ makeKeyAndOrderFront:NSApp];
    [native_window_ setDelegate:[[WindowDelegate alloc] init:this]];
    if (description.is_minimize)
    {
        [native_window_ miniaturize:nullptr];
    }

    if (parent)
    {
        [native_window_ setParentWindow: parent->native_window_];
        can_be_primary_ = false;
    }

    initialized_ = true;
}

void MacWindow::deinitialize()
{
    if (initialized_)
    {
        if (native_window_)
        {
            [native_window_ close];
            native_window_ = nullptr;
        }

        initialized_ = false;
    }
}

void MacWindow::destroy()
{
    if (initialized_)
    {
        if (native_window_)
        {
            [native_window_ setDelegate:nil]; // we don't need route event to window delegate
        }
        deinitialize();
        on_window_destroyed_.broadcast(shared_from_this());
    }
}

} // namespace atlas
