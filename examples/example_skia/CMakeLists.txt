cmake_minimum_required(VERSION 3.10)

# set the project name
project(examples_skia)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# add the executable
add_executable(examples_skia main.cpp
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
target_include_directories(examples_skia PUBLIC ../../)
target_include_directories(examples_skia PUBLIC ../../backends/)
