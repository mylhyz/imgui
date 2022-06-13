cmake_minimum_required(VERSION 3.10)

# set the project name
project(examples_apple_skia)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SKIA_INCLUDE_DIR "/Users/mylhyz/Google/skia/")
set(SKIA_SHARED_LIB "/Users/mylhyz/Google/skia/out/Debug_Shared/libskia.so")

find_library(COCOA_LIBRARY Cocoa)
find_library(METAL_LIBRARY Metal)
find_library(METAL_KIT_LIBRARY MetalKit)
find_library(GAME_CONTROLLER_LIBRARY GameController)

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
# add imgui osx and metal backend impl
../../backends/imgui_impl_metal.mm
../../backends/imgui_impl_osx.mm
)
target_compile_options(examples_apple_skia PRIVATE -ObjC++ -fobjc-weak -fobjc-arc)
target_compile_definitions(examples_apple_skia PRIVATE -DTARGET_OS_OSX)
target_include_directories(examples_apple_skia PUBLIC ../../)
target_include_directories(examples_apple_skia PUBLIC ../../backends/)
target_include_directories(examples_apple_skia PUBLIC ${SKIA_INCLUDE_DIR})
target_link_libraries(examples_apple_skia PUBLIC ${SKIA_SHARED_LIB} ${COCOA_LIBRARY} ${METAL_LIBRARY} ${METAL_KIT_LIBRARY} ${GAME_CONTROLLER_LIBRARY})
