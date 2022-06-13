cmake_minimum_required(VERSION 3.10)

# set the project name
project(examples_win32_skia)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SKIA_INCLUDE_DIR "C:\\Work\\skia_build_win32\\skia")
add_library(skia SHARED IMPORTED)
set_property(TARGET skia PROPERTY IMPORTED_LOCATION "C:\\Work\\skia_build_win32\\skia\\out\\MSVC_x86\\skia.dll")
set_property(TARGET skia PROPERTY IMPORTED_IMPLIB "C:\\Work\\skia_build_win32\\skia\\out\\MSVC_x86\\skia.dll.lib")

# add the executable
add_executable(examples_win32_skia main.cpp
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
../../backends/imgui_impl_dx9.cpp
../../backends/imgui_impl_win32.cpp
)
target_include_directories(examples_win32_skia PUBLIC ../../)
target_include_directories(examples_win32_skia PUBLIC ../../backends/)
target_include_directories(examples_win32_skia PUBLIC ${SKIA_INCLUDE_DIR})
target_link_libraries(examples_win32_skia PUBLIC skia)
target_compile_definitions(examples_win32_skia PUBLIC -D_UNICODE -DUNICODE -D_WINDOWS)
target_link_options(examples_win32_skia PUBLIC "/SUBSYSTEM:WINDOWS,5.01")
