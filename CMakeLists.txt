cmake_minimum_required(VERSION 3.29)
project(apep)

set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 REQUIRED)

# Find OpenGL
find_package(OpenGL REQUIRED)

include_directories(submodules/imgui)
include_directories(submodules/imgui/backends)
include_directories(submodules/implot)

add_executable(apep
        src/main.cpp
        submodules/imgui/imgui.cpp
        submodules/imgui/imgui_draw.cpp
        submodules/imgui/imgui_widgets.cpp
        submodules/imgui/imgui_tables.cpp
        submodules/imgui/backends/imgui_impl_glfw.cpp
        submodules/imgui/backends/imgui_impl_opengl3.cpp
        submodules/implot/implot.cpp
        submodules/implot/implot_items.cpp
)

target_link_libraries(apep PUBLIC glfw OpenGL::GL)
