cmake_minimum_required(VERSION 3.10)

# set the project name
project(examples_apple_skia)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SKIA_INCLUDE_DIR "/Users/mylhyz/Google/skia_build/skia")
set(SKIA_LIB "/Users/mylhyz/Google/skia_build/skia/out/Debug/libskia.a")

find_library(COCOA_LIBRARY Cocoa)
find_library(METAL_LIBRARY Metal)
find_library(METAL_KIT_LIBRARY MetalKit)
find_library(GAME_CONTROLLER_LIBRARY GameController)

find_package(OpenGL)

# add the executable
add_executable(examples_apple_skia main.mm
# add imgui core
../../imgui.cpp
../../imgui_draw.cpp
../../imgui_tables.cpp
../../imgui_widgets.cpp
# add imgui demo windows
../../imgui_demo.cpp
# add imgui backend impl
../../backends/imgui_impl_skia.cpp
# add imgui osx backend impl
../../backends/imgui_impl_osx.mm
)
target_compile_options(examples_apple_skia PRIVATE -ObjC++ -fobjc-weak -fobjc-arc)
# 预定义宏的作用
# SK_GL 用于提示Skia头文件，声明使用了GL编译
# TARGET_OS_OSX 用于 imgui_impl_metal 中，声明是运行在MacOSX中
# GR_GL_CHECK_ERROR=0 关闭GL命令检查
# GR_GL_LOG_CALLS=0 关闭GL命令log
target_compile_definitions(examples_apple_skia PRIVATE -DSK_GAMMA_APPLY_TO_A8 -DSK_ALLOW_STATIC_GLOBAL_INITIALIZERS=1 -DGR_TEST_UTILS=1 -DSK_GL -DSK_ENABLE_DUMP_GPU -DSK_SUPPORT_GPU=1 -DSK_SUPPORT_PDF -DSK_CODEC_DECODES_JPEG -DSK_ENCODE_JPEG -DSK_ENABLE_ANDROID_UTILS -DSK_USE_LIBGIFCODEC -DSK_HAS_HEIF_LIBRARY -DSK_CODEC_DECODES_PNG -DSK_ENCODE_PNG -DSK_CODEC_DECODES_RAW -DSKVM_JIT_WHEN_POSSIBLE -DSK_CODEC_DECODES_WEBP -DSK_ENCODE_WEBP -DSK_XML -DSK_ENABLE_SKSL -DSK_ASSUME_GL=1 -DSK_ENABLE_API_AVAILABLE -DSK_ENABLE_SKOTTIE -DSK_ENABLE_SVG -DIMGUI_ENABLE_OSX_DEFAULT_CLIPBOARD_FUNCTIONS)
target_include_directories(examples_apple_skia PUBLIC ../../)
target_include_directories(examples_apple_skia PUBLIC ../../backends/)
target_include_directories(examples_apple_skia PUBLIC ${OPENGL_INCLUDE_DIR})
target_include_directories(examples_apple_skia PUBLIC ${SKIA_INCLUDE_DIR})
target_link_libraries(examples_apple_skia PUBLIC ${SKIA_LIB} ${COCOA_LIBRARY} ${METAL_LIBRARY} ${METAL_KIT_LIBRARY} ${GAME_CONTROLLER_LIBRARY} ${OPENGL_LIBRARIES})
