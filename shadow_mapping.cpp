//
// Created by petr on 11/30/19.
//

#include "shadow_mapping.h"
#include "rendering/Data.h"
#include "rendering/shading/ShadowMap.h"
#include "rendering/utils/DrawTexture.h"
#include "ui/elements.h"
#include "ui/managers.h"
#include "various/loc_assert.h"
#include <SDL2CPP/MainLoop.h>
#include <SDL2CPP/Window.h>
#include <String.h>
#include <config/JsonConfig.h>
#include <gl_utils.h>
#include <rendering/ModelRenderer.h>

using Conf = JsonConfig<true>;

void prepModels(ModelRenderer &modelRenderer, const std::string &assetPath) {
  ObjModelLoader modelLoader{assetPath + "/models"};
  modelRenderer.addModel(modelLoader.loadModel("cube", "cube1"))
      .setPosition({0, 2, 0})
      .setDrawn(true)
      .setScale({0.1, 0.1, 0.1});
  modelRenderer.addModel(modelLoader.loadModel("cube", "cube2"))
      .setPosition({5, 3, 5})
      .setDrawn(true)
      .setScale({0.5, 0.5, 0.5});

  for (auto x : range(-15, 15, 3)) {
    for (auto y : range(-15, 15, 3)) {
      modelRenderer
          .addModel(modelLoader.loadModel("cube", "cube" + std::to_string(x) +
                                                      std::to_string(y)))
          .setPosition({static_cast<float>(x), 3, static_cast<float>(y)});
    }
  }
  modelRenderer.addModel(modelLoader.loadModel("sphere", "b"))
      .setScale(3, 3, 3)
      .setPosition(0, 5, 0);

  modelRenderer.addModel(modelLoader.loadModel("floor", "floor1"))
      .setPosition({0, -5, 0});
  modelRenderer.addModel(modelLoader.loadModel("wall", "wall1"))
      .setPosition({-5, -5.1, 0});

  modelRenderer.addModel(modelLoader.loadModel("wall", "wall2"))
      .setPosition({-5, -5.1, 0})
      .setScale({5, 5, 20});

  modelRenderer.addModel(modelLoader.loadModel("sphere", "light"))
      .setDrawn(false);
}

void main_shadow_mapping(int argc, char *argv[]) {
  using namespace sdl2cpp::ui;
  loc_assert(argc != 1, "Provide path for config");
  Conf config{argv[1]};

  auto mainLoop = std::make_shared<sdl2cpp::MainLoop>();

  const auto deviceData = config.get<DeviceData>("device").value();
  auto window = std::make_shared<sdl2cpp::Window>(deviceData.screen.width,
                                                  deviceData.screen.height);
  window->createContext("rendering", 430);
  mainLoop->addWindow("mainWindow", window);

  // init OpenGL
  ge::gl::init(SDL_GL_GetProcAddress);
  ge::gl::setHighDebugMessage();

  ge::gl::glClearColor(0, 0, 0, 1);

  setShaderLocation(config.get<std::string>("paths", "shaderLocation").value());

  const auto assetPath =
      config.get<std::string>("paths", "assetsLocation").value();

  ModelRenderer modelRenderer;
  prepModels(modelRenderer, assetPath);

  sdl2cpp::ui::UIManager uiManager{window, String{assetPath + "/gui/fonts"}};
  auto cameraController =
      uiManager.createGUIObject<sdl2cpp::ui::CameraController>(
          glm::vec3{0, 0, 0}, glm::vec3{1920, 1080, 0});

  bool showFrameBuffer = false;
  auto btn = uiManager.createGUIObject<sdl2cpp::ui::Button>(
      glm::vec3{0, 0, 1}, glm::vec3{300, 100, 0});
  btn->text.setText(L"show/hide light sphere"_sw);
  btn->text.setFont("arialbd", 10);

  btn->setMouseClicked([&modelRenderer](sdl2cpp::ui::EventInfo,
                                        sdl2cpp::ui::MouseButton, SDL_Point) {
    auto model = modelRenderer.modelById("light").value();
    model->setDrawn(!model->isDrawn());
  });

  auto btn2 = uiManager.createGUIObject<sdl2cpp::ui::Button>(
      glm::vec3{0, 110, 1}, glm::vec3{300, 100, 0});

  btn2->setMouseClicked(
      [&showFrameBuffer](sdl2cpp::ui::EventInfo, sdl2cpp::ui::MouseButton,
                         SDL_Point) { showFrameBuffer = !showFrameBuffer; });
  const float near_plane = .1f;
  const float far_plane = 70.5f;
  const glm::mat4 lightProjection =  //glm::perspective(glm::radians(140.f),
                                     //(float)1024 / (float)1024, near_plane, far_plane);
      glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
  ShadowMap sm{lightProjection, {0.f, 5.0f, 0.0}, {0.0, 0, 0}, 4096, 4096};

  DrawTexture drawTexture;

  auto btn3 = uiManager.createGUIObject<sdl2cpp::ui::Button>(
      glm::vec3{0, 220, 1}, glm::vec3{300, 100, 0});
  btn3->setMouseClicked([&sm, &modelRenderer](sdl2cpp::ui::EventInfo,
                                              sdl2cpp::ui::MouseButton,
                                              SDL_Point) {
    sm.setLightPos(sm.getLightPos() + glm::vec3{0, 1, 0});
    auto model = modelRenderer.modelById("light").value();
    model->setPosition(sm.getLightPos());
  });

  btn2->text.setText(L"show/hide depth texture"_sw);
  btn2->text.setFont("arialbd", 10);

  auto btn4 = uiManager.createGUIObject<sdl2cpp::ui::Button>(
      glm::vec3{0, 340, 1}, glm::vec3{300, 500, 0});

  bool down = false;
  btn4->setMouseDown([&down] { down = true; })
      .setMouseUp([&down] { down = false; })
      .setMouseMove([&down, &sm, &modelRenderer](EventInfo, SDL_Point oldPos,
                                                 SDL_Point newPos) {
        if (!down) {
          return;
        }
        glm::vec3 delta{oldPos.x - newPos.x, oldPos.y - newPos.y, 0.0};
        sm.setLightPos(sm.getLightPos() + delta / 10.0f);
        auto model = modelRenderer.modelById("light").value();
        model->setPosition(sm.getLightPos());
      })
      .setMouseWheel([&down, &sm, &modelRenderer](
                         EventInfo, ScrollDirection dir, int delta) {
        if (!down) {
          return;
        }
        glm::vec3 d{0};
        switch (dir) {
        case ScrollDirection::Left:
          break;
        case ScrollDirection::Right:
          break;
        case ScrollDirection::Up:
          d.z += delta;
          break;
        case ScrollDirection::Down:
          d.z += delta;
          break;
        }
        sm.setLightPos(sm.getLightPos() + d);
        auto model = modelRenderer.modelById("light").value();
        model->setPosition(sm.getLightPos());
      });

  // ge::gl::glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  auto projection =
      glm::perspective(glm::radians(60.f), 1920.f / 1080, 0.1f, 10000.0f);
  auto renderProgram = std::make_shared<ge::gl::Program>(
      "shadow_map/render"_vert, "shadow_map/render"_frag);
  mainLoop->setIdleCallback([&]() {
    ge::gl::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ge::gl::glEnable(GL_DEPTH_TEST);

    sm.begin();
    ge::gl::glEnable(GL_CULL_FACE);
    ge::gl::glCullFace(GL_FRONT);
    modelRenderer.render(sm.getProgram(), cameraController->getViewMatrix(),
                         false);
    ge::gl::glCullFace(GL_BACK);
    sm.end();

    if (showFrameBuffer) {
      drawTexture.draw(sm.getDepthMap().getId());
    } else {
      renderProgram->use();
      ge::gl::glActiveTexture(GL_TEXTURE0);
      ge::gl::glBindTexture(GL_TEXTURE_2D, sm.getDepthMap().getId());

      ge::gl::glUniform1i(
          ge::gl::glGetUniformLocation(renderProgram->getId(), "shadowMap"), 0);

      renderProgram->set3fv("lightPos", &sm.getLightPos()[0]);
      renderProgram->set3fv("viewPos", &cameraController->getPosition()[0]);
      renderProgram->setMatrix4fv("projection", &projection[0][0]);
      renderProgram->setMatrix4fv("lightSpaceMatrix",
                                  &sm.getLightSpaceMatrix()[0][0]);
      modelRenderer.render(renderProgram, cameraController->getViewMatrix(),
                           true);
    }
    ge::gl::glDisable(GL_DEPTH_TEST);
    uiManager.render(glm::ortho<float>(0, 1000, 0, 562.5, -1, 1));

    window->swap();
  });

  (*mainLoop)();
}
