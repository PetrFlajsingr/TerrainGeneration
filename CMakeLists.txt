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


include_directories(include)
include_directories(utils)
include_directories(.)

file(GLOB_RECURSE ui_src "ui/*.cpp")
file(GLOB_RECURSE utils_src "utils/*.cpp")

add_executable(TerrainGeneration main.cpp ${ui_src} ${utils_src}
        ui/elements/CameraController.cpp rendering/archive/DensityGenerators.cpp
        rendering/archive/OldChunk.cpp rendering/ChunkManager.cpp rendering/ChunkManager.h
        rendering/Chunk.cpp rendering/Chunk.h rendering/Light.cpp rendering/Light.h rendering/Data.h rendering/Data.cpp
        ui/interface/KeyboardInteractable.cpp ui/elements/KeyAction.cpp ui/elements/KeyAction.h ui/interface/MouseInteractable.cpp ui/interface/MouseInteractable.h utils/various/StringUtils.cpp ui/GUIRenderer.cpp ui/GUIRenderer.h)

target_link_libraries(TerrainGeneration SDL2_image geGL::geGL SDL2CPP::SDL2CPP glm)