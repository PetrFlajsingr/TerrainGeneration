//
// Created by petr on 11/30/19.
//

#include "shadow_mapping.h"
#include "rendering/Data.h"
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

  modelRenderer.addModel(modelLoader.loadModel("floor", "floor1"))
      .setPosition({0, -5, 0});
  modelRenderer.addModel(modelLoader.loadModel("wall", "wall1"))
      .setPosition({-5, -5.1, 0});

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

  ShadowMapData sm;

  DrawFrameBuffer drawFrameBuffer;

  sm.lightPos = {-5.0f, 2.0f, -10.0};

  auto btn3 = uiManager.createGUIObject<sdl2cpp::ui::Button>(
      glm::vec3{0, 220, 1}, glm::vec3{300, 100, 0});
  btn3->setMouseClicked([&sm, &modelRenderer](sdl2cpp::ui::EventInfo,
                                              sdl2cpp::ui::MouseButton,
                                              SDL_Point) {
    sm.lightPos += glm::vec3{0, 1, 0};
    auto model = modelRenderer.modelById("light").value();
    model->setPosition(sm.lightPos);
  });

  btn2->text.setText(L"show/hide depth texture"_sw);
  btn2->text.setFont("arialbd", 10);

  auto btn4 = uiManager.createGUIObject<sdl2cpp::ui::Button>(
      glm::vec3{0, 340, 1}, glm::vec3{300, 500, 0});

  bool down = false;
  btn4->setMouseDown([&down] {
        down = true;
      })
      .setMouseUp([&down] {
        down = false;
      })
      .setMouseMove([&down, &sm, &modelRenderer](EventInfo, SDL_Point oldPos,
                                                 SDL_Point newPos) {
        if (!down) {
          return;
        }
        glm::vec3 delta{oldPos.x - newPos.x, oldPos.y - newPos.y, 0.0};
        sm.lightPos += delta / 10.0f;
        auto model = modelRenderer.modelById("light").value();
        model->setPosition(sm.lightPos);
      });

  // ge::gl::glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  auto projection =
      glm::perspective(glm::radians(60.f), 1920.f / 1080, 0.1f, 100.0f);
  mainLoop->setIdleCallback([&]() {
    ge::gl::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ge::gl::glEnable(GL_DEPTH_TEST);

    sm.begin();
    ge::gl::glEnable(GL_CULL_FACE);
    ge::gl::glCullFace(GL_FRONT);
    modelRenderer.render(sm.program, cameraController->getViewMatrix(), false);
    ge::gl::glCullFace(GL_BACK);
    sm.end();

    if (showFrameBuffer) {
      drawFrameBuffer.program.use();
      // drawFrameBuffer.program.set("near_plane", sm.near_plane);
      // drawFrameBuffer.program.set("far_plane", sm.far_plane);
      ge::gl::glBindVertexArray(drawFrameBuffer.quadVAO);
      ge::gl::glBindTexture(GL_TEXTURE_2D, sm.depthMap);
      ge::gl::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    } else {
      sm.renderProgram->use();
      ge::gl::glActiveTexture(GL_TEXTURE0);
      ge::gl::glBindTexture(GL_TEXTURE_2D, sm.depthMap);

      ge::gl::glUniform1i(
          ge::gl::glGetUniformLocation(sm.renderProgram->getId(), "shadowMap"),
          0);

      sm.renderProgram->set3fv("lightPos", &sm.lightPos[0]);
      sm.renderProgram->set3fv("viewPos", &cameraController->getPosition()[0]);
      sm.renderProgram->setMatrix4fv("projection", &projection[0][0]);
      sm.renderProgram->setMatrix4fv("lightSpaceMatrix",
                                     &sm.lightSpaceMatrix[0][0]);
      modelRenderer.render(sm.renderProgram, cameraController->getViewMatrix(),
                           true);
    }
    ge::gl::glDisable(GL_DEPTH_TEST);
    uiManager.render(glm::ortho<float>(0, 1000, 0, 562.5, -1, 1));

    sm.lightPos += glm::vec3{0.001, 0, 0};
    window->swap();
  });

  (*mainLoop)();
}

DrawFrameBuffer::DrawFrameBuffer() {
  float quadVertices[] = {
      // positions        // texture Coords
      -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
      1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
  };
  ge::gl::glGenVertexArrays(1, &quadVAO);
  ge::gl::glGenBuffers(1, &quadVBO);
  ge::gl::glBindVertexArray(quadVAO);
  ge::gl::glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  ge::gl::glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices,
                       GL_STATIC_DRAW);
  ge::gl::glEnableVertexAttribArray(0);
  ge::gl::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                                (void *)0);
  ge::gl::glEnableVertexAttribArray(1);
  ge::gl::glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                                (void *)(3 * sizeof(float)));
}

ShadowMapData::ShadowMapData() {
  ge::gl::glGenTextures(1, &depthMap);
  ge::gl::glBindTexture(GL_TEXTURE_2D, depthMap);
  ge::gl::glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0,
                       GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  ge::gl::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  ge::gl::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  ge::gl::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  ge::gl::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  float borderColor[] = {1.0, 1.0, 1.0, 1.0};
  ge::gl::glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

  ge::gl::glGenFramebuffers(1, &depthMapFBO);
  ge::gl::glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  ge::gl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                 GL_TEXTURE_2D, depthMap, 0);
  ge::gl::glDrawBuffer(GL_NONE);
  ge::gl::glReadBuffer(GL_NONE);
  ge::gl::glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void ShadowMapData::begin() {
  ge::gl::glGetIntegerv(GL_VIEWPORT, m_viewport);

  program->use();
  ge::gl::glViewport(0, 0, width, height);
  // fbo.bind(GL_FRAMEBUFFER);
  ge::gl::glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  ge::gl::glClear(GL_DEPTH_BUFFER_BIT);
  // fbo.clear(GL_DEPTH_BUFFER_BIT);

  glm::mat4 lightView = glm::lookAt(lightPos, target, up);

  lightSpaceMatrix = lightProjection * lightView;
  program->setMatrix4fv("lightSpaceMatrix", &lightSpaceMatrix[0][0]);
}
void ShadowMapData::end() {
  // fbo.unbind(GL_FRAMEBUFFER);
  ge::gl::glBindFramebuffer(GL_FRAMEBUFFER, 0);
  auto [x, y, width, height] = m_viewport;
  ge::gl::glViewport(x, y, width, height);
}
