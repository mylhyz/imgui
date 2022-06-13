cmake_minimum_required(VERSION 3.10)

# set the project name
project(examples_apple_skia)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SKIA_INCLUDE_DIR "/Users/mylhyz/Google/skia/")
set(SKIA_SHARED_LIB "/Users/mylhyz/Google/skia/out/Debug_Shared/libskia.so")

# add the executable
add_executable(examples_apple_skia main.cpp
# add imgui core
../../imgui.cpp
../../imgui_draw.cpp
../../imgui_tables.cpp
../../imgui_widgets.cpp
# add imgui demo windows
../../imgui_demo.cpp
# add imgui backend impl
../../backends/imgui_impl_skia.cpp
)
target_include_directories(examples_apple_skia PUBLIC ../../)
target_include_directories(examples_apple_skia PUBLIC ../../backends/)
target_include_directories(examples_apple_skia PUBLIC ${SKIA_INCLUDE_DIR})
target_link_libraries(examples_apple_skia PUBLIC ${SKIA_SHARED_LIB})
