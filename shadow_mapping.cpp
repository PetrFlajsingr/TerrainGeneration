//
// Created by petr on 11/30/19.
//

#define OLD_SM

#include "shadow_mapping.h"
#include "rendering/Data.h"

#ifdef OLD_SM
#include "rendering/shading/ShadowMap.h"
#else
#include "rendering/shading/CascadedShadowMap.h"
#endif

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
#include <rendering/SceneLoader.h>

using Conf = JsonConfig<true>;

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

  modelRenderer.loadScene(SceneLoader{assetPath, "scene1"});
  std::vector<GraphicsModelBase *> spheres;
  std::vector<GraphicsModelBase *> cubes;
  GraphicsModelBase *bigSphere = modelRenderer.modelById("bigSphere")->get();
  {
    ObjModelLoader modelLoader(assetPath + "/models");
    modelRenderer.addModel(modelLoader.loadModel("sphere", "light"))
        .setDrawn(false);
  }

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
  btn2->text.setText(L"show/hide depth texture"_sw);
  btn2->text.setFont("arialbd", 10);

  auto btn4 = uiManager.createGUIObject<sdl2cpp::ui::Button>(
      glm::vec3{0, 340, 1}, glm::vec3{300, 500, 0});
  bool down = false;
#ifdef OLD_SM
  const float near_plane = .1f;
  const float far_plane = 70.5f;
  const glm::mat4
      lightProjection =
      glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);

  ShadowMap sm{lightProjection, {0.f, 5.0f, 0.0}, {0.0, 0, 0}, 4096, 4096};

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
#else
  CascadedShadowMap cascadedShadowMap{3, 4096, 4096};
  cascadedShadowMap.setLightPos({1, 10, 5});
  cascadedShadowMap.setTarget({0, 0, 0});
  btn4->setMouseDown([&down] { down = true; })
      .setMouseUp([&down] { down = false; })
      .setMouseMove([&down, &cascadedShadowMap, &modelRenderer](
                        EventInfo, SDL_Point oldPos, SDL_Point newPos) {
        if (!down) {
          return;
        }
        glm::vec3 delta{oldPos.x - newPos.x, oldPos.y - newPos.y, 0.0};
        cascadedShadowMap.setLightPos(cascadedShadowMap.getLightPos() +
                                      delta / 10.0f);
        auto model = modelRenderer.modelById("light").value();
        model->setPosition(cascadedShadowMap.getLightPos());
      })
      .setMouseWheel([&down, &cascadedShadowMap, &modelRenderer](
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
        cascadedShadowMap.setLightPos(cascadedShadowMap.getLightPos() + d);
        auto model = modelRenderer.modelById("light").value();
        model->setPosition(cascadedShadowMap.getLightPos());
      });
#endif
  DrawTexture drawTexture;
  const auto near = 0.1f;
  const auto far = 500.0f;
  const auto aspectRatio = 1920.f / 1080;
  const auto fieldOfView = 60.0f;
  auto projection =
      glm::perspective(glm::radians(fieldOfView), aspectRatio, near, far);
  auto renderProgram = std::make_shared<ge::gl::Program>(
      "shadow_map/render"_vert, "shadow_map/render"_frag);

  bool shrink = true;

  int cnt = 0;
  while (true) {
    auto model = modelRenderer.modelById("sphere" + std::to_string(cnt));
    if (!model.has_value()) {
      break;
    }
    spheres.emplace_back(model.value().get());
    ++cnt;
  }

  cnt = 0;
  while (true) {
    auto model = modelRenderer.modelById("cube" + std::to_string(cnt));
    if (!model.has_value()) {
      break;
    }
    cubes.emplace_back(model.value().get());
    ++cnt;
  }

  mainLoop->setIdleCallback([&]() {
    ge::gl::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ge::gl::glEnable(GL_DEPTH_TEST);

    auto pos = bigSphere->getPosition();

    for (auto cube : cubes) {
      cube->setRotation(cube->getRotation() + glm::vec3{0.f, 0.3, 0});
    }

    if (shrink) {
      for (auto sphere : spheres) {
        auto oldPos = sphere->getPosition();
        sphere->setPosition(oldPos.x * 0.95f, oldPos.y, oldPos.z * 0.95f);
      }
      if (spheres.size() > 0 && spheres[0]->getPosition().x > -0.1) {
        shrink = false;
      }
      bigSphere->setPosition(pos.x + 0.2f, pos.y, pos.z);
    } else {
      for (auto sphere : spheres) {
        auto oldPos = sphere->getPosition();
        sphere->setPosition(oldPos.x * 1.05f, oldPos.y, oldPos.z * 1.05f);
      }
      if (spheres.size() > 0 && spheres[0]->getPosition().x < -50) {
        shrink = true;
      }
      bigSphere->setPosition(pos.x - 0.2f, pos.y, pos.z);
    }

#ifdef OLD_SM
    sm.begin();
    ge::gl::glEnable(GL_CULL_FACE);
    ge::gl::glCullFace(GL_FRONT);
    modelRenderer.render(sm.getProgram(), cameraController->getViewMatrix(),
                         false);
    ge::gl::glCullFace(GL_BACK);
    sm.end();
    ge::gl::glDisable(GL_CULL_FACE);

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
#else
    ge::gl::glEnable(GL_CULL_FACE);
    ge::gl::glCullFace(GL_FRONT);
    auto renderFnc = [&modelRenderer, &cameraController] (const std::shared_ptr<ge::gl::Program> &program) {
      modelRenderer.render(program, cameraController->getViewMatrix(),
                           false);
    };

    cascadedShadowMap.renderShadowMap(renderFnc,
                                      cameraController->getViewMatrix(), near,
                                      far, aspectRatio, fieldOfView);

    ge::gl::glCullFace(GL_BACK);

    if (showFrameBuffer) {
      drawTexture.draw(cascadedShadowMap.getDepthMaps()[0]->getId());
    } else {
      renderProgram->use();
      ge::gl::glActiveTexture(GL_TEXTURE0);
      ge::gl::glBindTexture(GL_TEXTURE_2D, cascadedShadowMap.getDepthMaps()[0]->getId());
      ge::gl::glUniform1i(
          ge::gl::glGetUniformLocation(renderProgram->getId(), "shadowMap"), 0);

      renderProgram->set3fv("lightPos", &cascadedShadowMap.getLightPos()[0]);
      renderProgram->set3fv("viewPos", &cameraController->getPosition()[0]);
      renderProgram->setMatrix4fv("projection", &projection[0][0]);
      renderProgram->setMatrix4fv("lightSpaceMatrix",
                                  &cascadedShadowMap.lightSpaceMatrix(0)[0][0]);
      modelRenderer.render(renderProgram, cameraController->getViewMatrix(),
                           true);
    }

#endif
    ge::gl::glDisable(GL_DEPTH_TEST);
    uiManager.render(glm::ortho<float>(0, 1000, 0, 562.5, -1, 1));

    window->swap();
  });

  (*mainLoop)();
}
