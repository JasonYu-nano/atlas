// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#import "engine_main.hpp"
#import "engine.hpp"

using namespace atlas;

static int32 g_engine_main_error_code = 0;

@interface AppDelegate : NSObject<NSApplicationDelegate>

@end

@implementation AppDelegate

- (id)init {

    if ( self = [super init] )
    {
    }
    return self;
}

- (void)applicationWillFinishLaunching:(NSNotification *)notification
{

}

- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
    g_engine_main_error_code = EngineMain(0, nullptr);
}

- (IBAction)requestQuit:(id)Sender
{
    if (g_engine)
    {
        g_engine->RequestShutdown();
    }
}

// handler for the quit apple event used by the Dock menu
- (void)handleQuitEvent:(NSAppleEventDescriptor*)Event withReplyEvent:(NSAppleEventDescriptor*)ReplyEvent
{
    [self requestQuit:self];
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)Sender;
{
    return NSTerminateNow;
}

@end

int main(int argc, const char* argv[])
{
    // create an autorelease pool
    @autoreleasepool {
        [NSApplication sharedApplication];
        [NSApp setDelegate:[AppDelegate new]];
        [NSApp run];
    }
    // execution never gets here
    return g_engine_main_error_code;
}