//
// Created by petr on 11/30/19.
//

#define OLD_SM0

#include "shadow_mapping.h"
#include "rendering/Data.h"

#ifdef OLD_SM
#include "rendering/shading/ShadowMap.h"
#else
#include "rendering/shadow_maps/CascadedShadowMap.h"
#endif

#include "rendering/utils/DrawTexture.h"
#include "ui/elements.h"
#include "ui/managers.h"
#include "various/loc_assert.h"
#include <SDL2CPP/MainLoop.h>
#include <SDL2CPP/Window.h>
#include <String.h>
#include <config/JsonConfig.h>
#include <graphics/gl_utils.h>
#include <rendering/Data.h>
#include <rendering/environment/EnvironmentRenderer.h>
#include <rendering/models/ModelRenderer.h>
#include <rendering/models/SceneLoader.h>
#include <rendering/textures/FileTextureLoader.h>

using Conf = JsonConfig<true>;

void main_shadow_mapping(int argc, char *argv[]) {
  using namespace sdl2cpp::ui;
  loc_assert(argc != 1, "Provide path for config");
  Conf config{argv[1]};

  auto mainLoop = std::make_shared<sdl2cpp::MainLoop>();

  const auto deviceData = config.get<DeviceData>("device").value();
  auto window = std::make_shared<sdl2cpp::Window>(deviceData.screen.width, deviceData.screen.height);
  window->createContext("rendering", 430);
  mainLoop->addWindow("mainWindow", window);

  // init OpenGL
  ge::gl::init(SDL_GL_GetProcAddress);
  ge::gl::setHighDebugMessage();

  ge::gl::glClearColor(0, 0, 0, 1);

  setShaderLocation(config.get<std::string>("paths", "shaderLocation").value());

  const auto assetPath = config.get<std::string>("paths", "assetsLocation").value();

  ObjModelLoader loader(assetPath + "/models/");
  EnvironmentRenderer envRenderer{loader};


  sdl2cpp::ui::UIManager uiManager{window, String{assetPath + "/gui/fonts"}};
  auto perspective = PerspectiveProjection(0.1f, 500000.f, 1920.f / 1080, 60.f);

  auto cameraController = uiManager.createGUIObject<sdl2cpp::ui::CameraController>(std::move(perspective), glm::vec3{0, 0, 0},
                                                                                   glm::vec3{1920, 1080, 0});
  cameraController->setMovementSpeed(50);

  FileTextureLoader texLoader{assetPath};
  TexOptions texOptions{GL_TEXTURE_2D,
                        GL_RGB,
                        true,
                        {
                            {GL_TEXTURE_WRAP_S, GL_REPEAT},
                            {GL_TEXTURE_WRAP_T, GL_REPEAT},
                            {GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR},
                            {GL_TEXTURE_MAG_FILTER, GL_LINEAR},
                        }};
  const auto near = 1.f;
  const auto far = 30000.0f;
  const auto aspectRatio = 1920.f / 1080;
  const auto fieldOfView = 45.0f;
  auto projection = glm::perspective(fieldOfView, aspectRatio, near, far);

  cameraController->camera.Position = {0, 25, 25};


  float time = 0;
  DrawTexture drawTexture;
  mainLoop->setIdleCallback([&]() {
    ge::gl::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ge::gl::glEnable(GL_DEPTH_TEST);


    ge::gl::glEnable(GL_CULL_FACE);

    ge::gl::glCullFace(GL_BACK);

    envRenderer.render(*cameraController, time);

    window->swap();
    time += 0.01;
  });

  (*mainLoop)();
}
