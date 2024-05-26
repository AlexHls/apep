cmake_minimum_required(VERSION 3.29)
project(apep)

set(CMAKE_CXX_STANDARD 20)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -march=native)
elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-Og -march=native)
endif ()

find_package(glfw3 REQUIRED)

# Find OpenGL
find_package(OpenGL REQUIRED)

# GLAD
add_subdirectory(submodules/glad)

# CXXOPTS
include_directories(submodules/cxxopts/include)

#########
# IMGUI #
#########

set(IMGUI_DIR submodules/imgui)
set(IMGUI_HEADERS
        ${IMGUI_DIR}/imgui.h
        ${IMGUI_DIR}/imconfig.h
        ${IMGUI_DIR}/imgui_internal.h
        ${IMGUI_DIR}/imstb_rectpack.h
        ${IMGUI_DIR}/imstb_textedit.h
        ${IMGUI_DIR}/imstb_truetype.h
        ${IMGUI_DIR}/backends/imgui_impl_glfw.h
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.h
        ${IMGUI_DIR}/backends/imgui_impl_opengl3_loader.h
        ${IMGUI_DIR}/misc/cpp/imgui_stdlib.h
)

set(IMGUI_SOURCES
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
        ${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp
)

add_library(imgui ${IMGUI_HEADERS} ${IMGUI_SOURCES})
target_link_libraries(imgui PUBLIC glfw glad OpenGL::GL)

include_directories(${IMGUI_DIR} ${IMGUI_DIR}/examples ${IMGUI_DIR}/examples/libs/gl3w ${IMGUI_DIR}/backends ${IMGUI_DIR}/misc/cpp)

##########
# IMPLOT #
##########

set(IMPLOT_DIR submodules/implot)
set(IMPLOT_HEADERS
        ${IMPLOT_DIR}/implot.h
        ${IMPLOT_DIR}/implot_internal.h
)
set(IMPLOT_SOURCES
        ${IMPLOT_DIR}/implot.cpp
        ${IMPLOT_DIR}/implot_demo.cpp
        ${IMPLOT_DIR}/implot_items.cpp
)

add_library(implot ${IMPLOT_HEADERS} ${IMPLOT_SOURCES})
target_link_libraries(implot PUBLIC imgui)
target_compile_definitions(implot PUBLIC IMPLOT_DEBUG IMPLOT_BACKEND_ENABLE_OPENGL3 IMGUI_IMPL_OPENGL_LOADER_GLAD)
set_property(TARGET implot PROPERTY CXX_STANDARD 11)
target_compile_options(implot PRIVATE -Wall -Wextra -pedantic -Werror -mavx2 -Ofast)
include_directories(${IMPLOT_DIR})

#################
# APP Framework #
#################
add_library(app
        src/app/App.h
        src/app/App.cpp
)
target_include_directories(app PUBLIC src/app)
target_link_libraries(app implot)

###################
# Hydro Framework #
###################
add_library(hydro
        src/hydro/Hydro.h
        src/hydro/Grid.h
        src/hydro/Grid.cpp
        src/hydro/Reconstruct.h
        src/hydro/Reconstruct.cpp
        src/hydro/RiemannSolver.h
        src/hydro/RiemannSolver.cpp
)
target_include_directories(hydro PUBLIC src/hydro)

#######################
# Individual programs #
#######################

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/utils)

add_executable(demo "src/demo.cpp")
target_link_libraries(demo PUBLIC app)

add_executable(rt_instability "src/rt_instability.cpp")
target_link_libraries(rt_instability PUBLIC app hydro)
