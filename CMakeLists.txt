cmake_minimum_required(VERSION 3.29)
project(apep)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-O3 -march=native)

find_package(glfw3 REQUIRED)

# Find OpenGL
find_package(OpenGL REQUIRED)

# GLAD
add_subdirectory(submodules/glad)

## IMGUI
#set(IMGUI_DIR submodules/imgui)
#set(IMGUI_HEADERS
#        ${IMGUI_DIR}/imgui.h
#        ${IMGUI_DIR}/imconfig.h
#        ${IMGUI_DIR}/imgui_internal.h
#        ${IMGUI_DIR}/imstb_rectpack.h
#        ${IMGUI_DIR}/imstb_textedit.h
#        ${IMGUI_DIR}/imstb_truetype.h
#        ${IMGUI_DIR}/imgui_tables.h
#        ${IMGUI_DIR}/imgui_impl_glfw.h
#        ${IMGUI_DIR}/imgui_impl_opengl3.h
#        ${IMGUI_DIR}/imgui_impl_opengl3_loader.h
#        ${IMGUI_DIR}/imgui_stdlib.h
#)
#
#set(IMGUI_SOURCES
#        ${IMGUI_DIR}/imgui_demo.cpp
#        ${IMGUI_DIR}/imgui.cpp
#        ${IMGUI_DIR}/imgui_draw.cpp
#        ${IMGUI_DIR}/imgui_widgets.cpp
#        ${IMGUI_DIR}/imgui_tables.cpp
#        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
#        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
#        ${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp
#)
#
#add_library(imgui ${IMGUI_HEADERS} ${IMGUI_SOURCES})
#target_link_libraries(imgui glfw glad OpenGL::GL)

include_directories(submodules/imgui)
include_directories(submodules/imgui/backends)

# IMPLOT


include_directories(submodules/implot)
include_directories(submodules/cxxopts/include)

add_executable(apep
        src/main.cpp
        submodules/imgui/imgui.cpp
        submodules/imgui/imgui_draw.cpp
        submodules/imgui/imgui_demo.cpp
        submodules/imgui/imgui_widgets.cpp
        submodules/imgui/imgui_tables.cpp
        submodules/imgui/backends/imgui_impl_glfw.cpp
        submodules/imgui/backends/imgui_impl_opengl3.cpp
        submodules/implot/implot.cpp
        submodules/implot/implot_demo.cpp
        submodules/implot/implot_items.cpp
        src/app/App.cpp
        src/app/App.h
)

target_link_libraries(apep PUBLIC glad glfw OpenGL::GL)
