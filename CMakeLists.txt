cmake_minimum_required(VERSION 3.29)
project(apep)

set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 REQUIRED)

# Find OpenGL
find_package(OpenGL REQUIRED)

include_directories(imgui)
include_directories(imgui/backends)

add_executable(apep
        src/main.cpp
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_tables.cpp
        imgui/backends/imgui_impl_glfw.cpp
        imgui/backends/imgui_impl_opengl3.cpp
)

target_link_libraries(apep PUBLIC glfw OpenGL::GL)
