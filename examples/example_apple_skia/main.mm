#import <Cocoa/Cocoa.h>
#include <OpenGL/gl.h>

#include "imgui.h"
#include "imgui_impl_skia.h"

//-----------------------------------------------------------------------------------
// AppView
//-----------------------------------------------------------------------------------

@interface AppView : NSView {
  NSTimer* animationTimer;
  NSOpenGLContext* fGLContext;
  NSOpenGLPixelFormat* fPixelFormat;
  sk_sp<GrDirectContext> fContext;
  sk_sp<const GrGLInterface> fBackendContext;
  sk_sp<SkSurface> fSurface;
  GrContextOptions fGrContextOptions;
  int fSampleCount;
  int fStencilBits;
  int fWidth;
  int fHeight;
}
@end

@implementation AppView

- (void)drawRect:(NSRect)dirtyRect {
  ImGui_Impl_Skia_NewFrame(dirtyRect.size.width, dirtyRect.size.height);

  // ImGui界面逻辑
  ImGui::NewFrame();

  // Our state (make them static = more or less global) as a convenience to keep the example terse.
  static bool show_demo_window = true;
  static bool show_another_window = false;
  static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can
  // browse its code to learn more about Dear ImGui!).
  if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

  // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named
  // window.
  {
    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Hello, world!");  // Create a window called "Hello, world!" and append into it.

    ImGui::Text(
        "This is some useful text.");  // Display some text (you can use a format strings too)
    ImGui::Checkbox("Demo Window",
                    &show_demo_window);  // Edit bools storing our window open/close state
    ImGui::Checkbox("Another Window", &show_another_window);

    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);  // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::ColorEdit3("clear color", (float*)&clear_color);  // Edit 3 floats representing a color

    if (ImGui::Button("Button"))  // Buttons return true when clicked (most widgets return true when
                                  // edited/activated)
      counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);
    ImGui::End();
  }

  // 3. Show another simple window.
  if (show_another_window) {
    ImGui::Begin(
        "Another Window",
        &show_another_window);  // Pass a pointer to our bool variable (the window will have a
                                // closing button that will clear the bool when clicked)
    ImGui::Text("Hello from another window!");
    if (ImGui::Button("Close Me")) show_another_window = false;
    ImGui::End();
  }

  // Rendering
  ImGui::Render();
  ImDrawData* draw_data = ImGui::GetDrawData();

  // 上屏
  ImGui_Impl_Skia_RenderDrawData(fSurface.get(),draw_data);

  // 触发View重绘
  if (!animationTimer)
    animationTimer = [NSTimer scheduledTimerWithTimeInterval:0.017
                                                      target:self
                                                    selector:@selector(animationTimerFired:)
                                                    userInfo:nil
                                                     repeats:YES];
}

- (instancetype)initWithFrame:(NSRect)frame {
  self = [super initWithFrame:frame];
  if (self) {
    //初始化 ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    //初始化 Skia Backends
    ImGui_Impl_Skia_Init();
  }
  return self;
}

- (void)dealloc {
  //销毁
  ImGui_Impl_Skia_Destroy();
  ImGui::DestroyContext();
}

- (void)animationTimerFired:(NSTimer*)timer {
  [self setNeedsDisplay:YES];
}

- (void)initializeContext {
  // 创建渲染上下文
  SkASSERT(!fContext);

  // 创建OpenGL上下文
  if (!fGLContext) {
    // set up pixel format
    constexpr int kMaxAttributes = 19;
    NSOpenGLPixelFormatAttribute attributes[kMaxAttributes];
    int numAttributes = 0;
    attributes[numAttributes++] = NSOpenGLPFAAccelerated;
    attributes[numAttributes++] = NSOpenGLPFAClosestPolicy;
    attributes[numAttributes++] = NSOpenGLPFADoubleBuffer;
    attributes[numAttributes++] = NSOpenGLPFAOpenGLProfile;
    attributes[numAttributes++] = NSOpenGLProfileVersion3_2Core;
    attributes[numAttributes++] = NSOpenGLPFAColorSize;
    attributes[numAttributes++] = 24;
    attributes[numAttributes++] = NSOpenGLPFAAlphaSize;
    attributes[numAttributes++] = 8;
    attributes[numAttributes++] = NSOpenGLPFADepthSize;
    attributes[numAttributes++] = 0;
    attributes[numAttributes++] = NSOpenGLPFAStencilSize;
    attributes[numAttributes++] = 8;
    //    if (fDisplayParams.fMSAASampleCount > 1) {
    //      attributes[numAttributes++] = NSOpenGLPFAMultisample;
    //      attributes[numAttributes++] = NSOpenGLPFASampleBuffers;
    //      attributes[numAttributes++] = 1;
    //      attributes[numAttributes++] = NSOpenGLPFASamples;
    //      attributes[numAttributes++] = fDisplayParams.fMSAASampleCount;
    //    } else {
    attributes[numAttributes++] = NSOpenGLPFASampleBuffers;
    attributes[numAttributes++] = 0;
    //    }
    attributes[numAttributes++] = 0;
    SkASSERT(numAttributes <= kMaxAttributes);

    fPixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
    if (nil == fPixelFormat) {
      return nullptr;
    }

    // create context
    fGLContext = [[NSOpenGLContext alloc] initWithFormat:fPixelFormat shareContext:nil];
    if (nil == fGLContext) {
      //      [fPixelFormat release];
      //      fPixelFormat = nil;
      return nullptr;
    }

    [self setWantsBestResolutionOpenGLSurface:YES];
    [fGLContext setView:self];
  }

  //  GLint swapInterval = fDisplayParams.fDisableVsync ? 0 : 1;
  GLint swapInterval = 1;
  [fGLContext setValues:&swapInterval forParameter:NSOpenGLCPSwapInterval];

  // make context current
  [fGLContext makeCurrentContext];

  glClearStencil(0);
  glClearColor(0, 0, 0, 255);
  glStencilMask(0xffffffff);
  glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  GLint stencilBits;
  [fPixelFormat getValues:&stencilBits forAttribute:NSOpenGLPFAStencilSize forVirtualScreen:0];
  fStencilBits = stencilBits;
  GLint sampleCount;
  [fPixelFormat getValues:&sampleCount forAttribute:NSOpenGLPFASamples forVirtualScreen:0];
  fSampleCount = sampleCount;
  fSampleCount = std::max(fSampleCount, 1);

  CGFloat backingScaleFactor = 2;
  fWidth = self.bounds.size.width * backingScaleFactor;
  fHeight = self.bounds.size.height * backingScaleFactor;
  glViewport(0, 0, fWidth, fHeight);

  fBackendContext = GrGLMakeNativeInterface();

  // 创建 GrDirectContext
  fContext = GrDirectContext::MakeGL(fBackendContext, fGrContextOptions);

  //创建Surface
  if (fContext) {
    GrGLint buffer;
    GR_GL_CALL(fBackendContext.get(), GetIntegerv(GR_GL_FRAMEBUFFER_BINDING, &buffer));

    GrGLFramebufferInfo fbInfo;
    fbInfo.fFBOID = buffer;
    fbInfo.fFormat = GR_GL_RGBA8;

    GrBackendRenderTarget backendRT(fWidth, fHeight, fSampleCount, fStencilBits, fbInfo);

    SkSurfaceProps fSurfaceProps(0, kRGB_H_SkPixelGeometry);
    sk_sp<SkColorSpace> fColorSpace;

    fSurface = SkSurface::MakeFromBackendRenderTarget(
        fContext.get(), backendRT, kBottomLeft_GrSurfaceOrigin, kRGBA_8888_SkColorType,
        fColorSpace, &fSurfaceProps);
  }
}

- (void)destroyContext {
  // 销毁渲染上下文
  fSurface.reset(nullptr);

  if (fContext) {
    // in case we have outstanding refs to this (lua?)
    fContext->abandonContext();
    fContext.reset();
  }

  fBackendContext.reset(nullptr);

  if (fGLContext) {
    [NSOpenGLContext clearCurrentContext];
  }
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
