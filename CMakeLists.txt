cmake_minimum_required(VERSION 3.15)
project(TerrainGeneration)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror=return-type -fconcepts -Wno-unknown-pragmas")

add_compile_definitions(GLT_IMPORTS)

include(FetchContent)

set(SDL2_DIR ~/Desktop/LIBS/SDL2/lib/cmake/SDL2)

find_package(SDL2 CONFIG REQUIRED)
find_package(Freetype REQUIRED)

FetchContent_Declare(SDL2CPP GIT_REPOSITORY https://github.com/dormon/SDL2CPP.git)
FetchContent_Declare(geGL GIT_REPOSITORY https://github.com/dormon/geGL.git)
FetchContent_Declare(fmt GIT_REPOSITORY https://github.com/fmtlib/fmt.git)
FetchContent_Declare(observable GIT_REPOSITORY https://github.com/ddinu/observable.git)
FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 0.9.8.5)

FetchContent_Declare(nlohmann_json
        URL https://github.com/nlohmann/json/releases/download/v3.1.2/include.zip
        )

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
FetchContent_GetProperties(fmt)
if (NOT fmt_POPULATED)
    FetchContent_Populate(fmt)
    add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR})
endif ()
FetchContent_GetProperties(observable)
if (NOT observable_POPULATED)
    FetchContent_Populate(observable)
    add_subdirectory(${observable_SOURCE_DIR} ${observable_BINARY_DIR})
endif ()
FetchContent_GetProperties(glm)
if (NOT glm_POPULATED)
    FetchContent_Populate(glm)
    add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR})
endif ()

FetchContent_GetProperties(nlohmann_json)
if (NOT nlohmann_json_POPULATED)
    FetchContent_Populate(nlohmann_json)
endif ()

include_directories(${fmt_SOURCE_DIR}/include)
include_directories(${observable_SOURCE_DIR}/observable/include)
include_directories(${glm_SOURCE_DIR})

add_library(nlohmann_json INTERFACE)

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

target_include_directories(nlohmann_json INTERFACE ${nlohmann_json_SOURCE_DIR})

add_executable(TerrainGeneration main.cpp ${ui_src} ${utils_src} ${ft_src} ${ft_src2}
        ${fmt_SOURCE_DIR}/src/format.cc
        ${fmt_SOURCE_DIR}/src/os.cc
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
        ui/interface/KeyboardInteractable.cpp ui/elements/KeyAction.cpp ui/elements/KeyAction.h ui/interface/MouseInteractable.cpp ui/interface/MouseInteractable.h utils/various/StringUtils.cpp ui/GUIRenderer.cpp ui/GUIRenderer.h ui/utils.cpp ui/utils.h utils/String.h ui/interface/Interactable.cpp ui/interface/Interactable.h include/gltext.cpp rendering/marching_cubes/Surroundings.cpp rendering/marching_cubes/Surroundings.h ui/text/TextRenderer.cpp ui/text/TextRenderer.h ui/text/FontManager.cpp ui/text/FontManager.h ui/Font.cpp ui/Font.h ui/interface/ui_traits.h ui/elements/Label.cpp ui/elements/Label.h utils/graphics/SparseBuffer.cpp utils/graphics/SparseBuffer.h ui/text/Text.cpp ui/text/Text.h rendering/models/ModelRenderer.cpp rendering/models/ModelRenderer.h marching_cubes.cpp marching_cubes.h ui/elements.h ui/managers.h rendering/textures/DrawTexture.cpp rendering/textures/DrawTexture.h rendering/shadow_maps/ShadowMap.cpp rendering/shadow_maps/ShadowMap.h ui/elements/Slider.cpp ui/elements/Slider.h ui/elements/TextArea.cpp ui/elements/TextArea.h rendering/shadow_maps/CascadedShadowMap.cpp rendering/shadow_maps/CascadedShadowMap.h rendering/models/SceneLoader.cpp rendering/models/SceneLoader.h utils/graphics/common.h ui/TimedEvent.h ui/TimedEvent.cpp rendering/models/GraphicsModelBase.cpp rendering/models/GraphicsModelBase.h rendering/models/GraphicsModel.cpp rendering/models/GraphicsModel.h rendering/models/InstancedGraphicsModel.cpp rendering/models/InstancedGraphicsModel.h utils/graphics/TempViewportSetter.cpp utils/graphics/TempViewportSetter.h rendering/marching_cubes/SurroundingsEnums.h rendering/marching_cubes/SurroundingsEnums.cpp utils/Colors.h rendering/marching_cubes/LODChunkController.cpp rendering/marching_cubes/LODChunkController.h rendering/marching_cubes/LODTypes.cpp rendering/marching_cubes/LODTypes.h rendering/marching_cubes/ChunkUsageManager.cpp rendering/marching_cubes/ChunkUsageManager.h ui/elements/Switch.cpp ui/elements/Switch.h include/stb_image.h rendering/textures/FileTextureLoader.cpp rendering/textures/FileTextureLoader.h rendering/environment/EnvironmentRenderer.cpp rendering/environment/EnvironmentRenderer.h MarchingCubesUI.cpp MarchingCubesUI.h rendering/marching_cubes/TerrainGenerationOptions.cpp rendering/marching_cubes/TerrainGenerationOptions.h)

target_link_libraries(TerrainGeneration geGL::geGL SDL2CPP::SDL2CPP
        ${FREETYPE_LIBRARIES}
        ${TINYOBJLOADER_LIBRARIES}
        nlohmann_json)


