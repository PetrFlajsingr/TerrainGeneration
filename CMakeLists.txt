cmake_minimum_required(VERSION 3.15)
project(TerrainGeneration)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

set(SDL2_DIR /home/petr/Desktop/pgpa/install/lib/cmake/SDL2)

find_package(glm  CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

FetchContent_Declare(SDL2CPP GIT_REPOSITORY https://github.com/dormon/SDL2CPP.git)
FetchContent_Declare(geGL    GIT_REPOSITORY https://github.com/dormon/geGL.git)

FetchContent_GetProperties(SDL2CPP)
if(NOT SDL2CPP_POPULATED)
    FetchContent_Populate(SDL2CPP)
    add_subdirectory(${sdl2cpp_SOURCE_DIR} ${sdl2cpp_BINARY_DIR})
endif()

FetchContent_GetProperties(geGL)
if(NOT geGL_POPULATED)
    FetchContent_Populate(geGL)
    add_subdirectory(${gegl_SOURCE_DIR} ${gegl_BINARY_DIR})
endif()

include_directories(third_party)
include_directories(utils)

add_executable(TerrainGeneration main.cpp gui/CameraController.cpp marching_cubes/DensityGenerators.cpp marching_cubes/shaders/GlslShaderLoader.cpp marching_cubes/Chunk.cpp utils/FPSCounter.cpp utils/Geometry.cpp utils/Geometry.h)

target_link_libraries(TerrainGeneration geGL::geGL SDL2CPP::SDL2CPP glm)