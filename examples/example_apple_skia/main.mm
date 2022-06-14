#import <Cocoa/Cocoa.h>

//-----------------------------------------------------------------------------------
// AppView
//-----------------------------------------------------------------------------------

@interface AppView : NSView
@end

@implementation AppView

-(void) drawRect:(NSRect)dirtyRect {
    //绘制
}

- (instancetype)init
{
    self = [super init];
    if (self) {
        //TODO 初始化
    }
    return self;
}

- (void)dealloc
{
    //TODO 销毁
}


@end

//-----------------------------------------------------------------------------------
// AppDelegate
//-----------------------------------------------------------------------------------

@interface AppDelegate : NSObject <NSApplicationDelegate>
@property(nonatomic, readonly) NSWindow* window;
@end

@implementation AppDelegate
@synthesize window = _window;

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)theApplication {
  return YES;
}

- (NSWindow*)window {
  if (_window != nil) return (_window);

  NSRect viewRect = NSMakeRect(100.0, 100.0, 100.0 + 1280.0, 100 + 720.0);

  _window = [[NSWindow alloc]
      initWithContentRect:viewRect
                styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskMiniaturizable |
                          NSWindowStyleMaskResizable | NSWindowStyleMaskClosable
                  backing:NSBackingStoreBuffered
                    defer:YES];
  [_window setTitle:@"Dear ImGui OSX+Skia Example"];
  [_window setAcceptsMouseMovedEvents:YES];
  [_window setOpaque:YES];
  [_window makeKeyAndOrderFront:NSApp];

  return (_window);
}

- (void)setupMenu {
  NSMenu* mainMenuBar = [[NSMenu alloc] init];
  NSMenu* appMenu;
  NSMenuItem* menuItem;

  appMenu = [[NSMenu alloc] initWithTitle:@"Dear ImGui OSX+Skia Example"];
  menuItem = [appMenu addItemWithTitle:@"Quit Dear ImGui OSX+Skia Example"
                                action:@selector(terminate:)
                         keyEquivalent:@"q"];
  [menuItem setKeyEquivalentModifierMask:NSEventModifierFlagCommand];

  menuItem = [[NSMenuItem alloc] init];
  [menuItem setSubmenu:appMenu];

  [mainMenuBar addItem:menuItem];

  appMenu = nil;
  [NSApp setMainMenu:mainMenuBar];
}

- (void)dealloc {
  _window = nil;
}

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification {
  // Make the application a foreground application (else it won't receive keyboard events)
  ProcessSerialNumber psn = {0, kCurrentProcess};
  TransformProcessType(&psn, kProcessTransformToForegroundApplication);

  // Menu
  [self setupMenu];

  // set content view
  AppView* view = [[AppView alloc] initWithFrame:self.window.frame];
  [self.window setContentView:view];
}

@end

//-----------------------------------------------------------------------------------
// Application main() function
//-----------------------------------------------------------------------------------

int main(int argc, const char* argv[]) {
  @autoreleasepool {
    NSApp = [NSApplication sharedApplication];
    AppDelegate* delegate = [[AppDelegate alloc] init];
    [[NSApplication sharedApplication] setDelegate:delegate];
    [NSApp run];
  }
  return NSApplicationMain(argc, argv);
}
