cmake_minimum_required(VERSION 3.15)
project(TerrainGeneration)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fconcepts")

add_compile_definitions(GLT_IMPORTS)

include(FetchContent)

set(SDL2_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/SDL2/lib/cmake/SDL2)

find_package(SDL2 CONFIG REQUIRED)
find_package(Freetype REQUIRED)

FetchContent_Declare(SDL2CPP GIT_REPOSITORY https://github.com/dormon/SDL2CPP.git)
FetchContent_Declare(geGL GIT_REPOSITORY https://github.com/dormon/geGL.git)

FetchContent_GetProperties(SDL2CPP)
if (NOT SDL2CPP_POPULATED)
    FetchContent_Populate(SDL2CPP)
    add_subdirectory(${sdl2cpp_SOURCE_DIR} ${sdl2cpp_BINARY_DIR})
endif ()

FetchContent_GetProperties(geGL)
if (NOT geGL_POPULATED)
    FetchContent_Populate(geGL)
    add_subdirectory(${gegl_SOURCE_DIR} ${gegl_BINARY_DIR})
endif ()


include_directories(include)
include_directories(utils)
include_directories(.)
include_directories(${FREETYPE_INCLUDE_DIRS})
include_directories(${GLM_INCLUDE_DIR})
include_directories(include/freetype-gl++/freetype-gl)

file(GLOB_RECURSE ui_src "ui/*.cpp")
file(GLOB_RECURSE utils_src "utils/*.cpp")
file(GLOB_RECURSE ft_src "include/freetype-gl++/*.cpp")
file(GLOB_RECURSE ft_src2 "include/freetype-gl++/freetype-gl/*.c")

add_executable(TerrainGeneration main.cpp ${ui_src} ${utils_src} ${ft_src} ${ft_src2}
        include/fmt/format.cc include/fmt/posix.cc
        include/gltext.cpp
        include/Camera.cpp
        utils/graphics/Geometry.cpp
        ui/elements/CameraController.cpp
        utils/graphics/ViewFrustum.cpp
        rendering/marching_cubes/ChunkManager.cpp rendering/marching_cubes/ChunkManager.h
        include/tinyxml2.cpp
        ui/elements/CameraController.cpp rendering/marching_cubes/ChunkManager.cpp rendering/marching_cubes/ChunkManager.h
        utils/graphics/Geometry.cpp
        utils/graphics/ViewFrustum.cpp
        rendering/marching_cubes/Chunk.cpp rendering/marching_cubes/Chunk.h rendering/Light.cpp rendering/Light.h rendering/Data.h rendering/Data.cpp
        ui/interface/KeyboardInteractable.cpp ui/elements/KeyAction.cpp ui/elements/KeyAction.h ui/interface/MouseInteractable.cpp ui/interface/MouseInteractable.h utils/various/StringUtils.cpp ui/GUIRenderer.cpp ui/GUIRenderer.h ui/utils.cpp ui/utils.h utils/String.h ui/interface/Interactable.cpp ui/interface/Interactable.h include/gltext.cpp rendering/marching_cubes/Surroundings.cpp rendering/marching_cubes/Surroundings.h rendering/text/TextRenderer.cpp rendering/text/TextRenderer.h rendering/text/FontManager.cpp rendering/text/FontManager.h ui/Font.cpp ui/Font.h ui/interface/ui_traits.h ui/elements/Label.cpp ui/elements/Label.h utils/SparseBuffer.cpp utils/SparseBuffer.h rendering/text/Text.cpp rendering/text/Text.h rendering/models/ModelRenderer.cpp rendering/models/ModelRenderer.h marching_cubes.cpp marching_cubes.h ui/elements.h ui/managers.h shadow_mapping.cpp shadow_mapping.h rendering/utils/DrawTexture.cpp rendering/utils/DrawTexture.h rendering/shading/ShadowMap.cpp rendering/shading/ShadowMap.h ui_main.cpp ui_main.h ui/elements/Slider.cpp ui/elements/Slider.h ui/elements/TextArea.cpp ui/elements/TextArea.h rendering/shading/CascadedShadowMap.cpp rendering/shading/CascadedShadowMap.h rendering/models/SceneLoader.cpp rendering/models/SceneLoader.h utils/graphics/common.h ui/TimedEvent.h ui/TimedEvent.cpp rendering/models/GraphicsModelBase.cpp rendering/models/GraphicsModelBase.h rendering/models/GraphicsModel.cpp rendering/models/GraphicsModel.h rendering/models/InstancedGraphicsModel.cpp rendering/models/InstancedGraphicsModel.h utils/TempViewportSetter.cpp utils/TempViewportSetter.h)

target_link_libraries(TerrainGeneration geGL::geGL SDL2CPP::SDL2CPP
        ${FREETYPE_LIBRARIES}
        ${TINYOBJLOADER_LIBRARIES})


