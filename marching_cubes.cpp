//
// Created by petr on 11/30/19.
//

#include "marching_cubes.h"
#include "rendering/Data.h"
#include "ui/elements.h"
#include "ui/managers.h"
#include "utils/config/JsonConfig.h"
#include "various/loc_assert.h"
#include <SDL2CPP/MainLoop.h>
#include <SDL2CPP/Window.h>
#include <gl_utils.h>
#include <rendering/ChunkManager.h>
#include <rendering/ModelRenderer.h>
#include <time/FPSCounter.h>
#include <types.h>

using namespace sdl2cpp::ui;
using Conf = JsonConfig<true>;

struct UI {
  std::shared_ptr<Button> lineFillBtn;
  std::shared_ptr<Label> fpsLbl;
  std::shared_ptr<Label> chunkInfoLbl;
  std::shared_ptr<CameraController> cameraController;
};

UI initUI(UIManager &uiManager) {
  auto cameraController = uiManager.createGUIObject<CameraController>(
      glm::vec3{0, 0, 0}, glm::vec3{1920, 1080, 0});

  auto lineFillBtn = uiManager.createGUIObject<sdl2cpp::ui::Button>(
      glm::vec3{0, 0, 1}, glm::vec3{250, 100, 0});
  lineFillBtn->text.setFont("arialbd", 60);
  lineFillBtn->text.setText(L"Line"_sw);

  auto fpsLbl = uiManager.createGUIObject<Label>(glm::vec3{1300, 0, 1},
                                                 glm::vec3{220, 20, 0});
  fpsLbl->text.setFont("arialbd", 10);

  auto chunkInfoLbl = uiManager.createGUIObject<Label>(glm::vec3{0, 1000, 1},
                                                       glm::vec3{500, 20, 0});
  chunkInfoLbl->text.setFont("arialbd", 10);
  chunkInfoLbl->text.setColor({1, 1, 1, 1});

  return {lineFillBtn, fpsLbl, chunkInfoLbl, cameraController};
}

void initModels(ModelRenderer &modelRenderer, const std::string &assetPath) {
  ObjModelLoader modelLoader{assetPath + "/models"};
  auto sphereModel = modelLoader.loadModel("sphere", "sphere1");
  modelRenderer.addModel(sphereModel);
  modelRenderer.addModel(modelLoader.loadModel("sphere", "sphere2"));
  modelRenderer.modelById("sphere2")
      .value()
      ->setPosition({500, 450, 500})
      .setScale({1, 1, 1});
  sphereModel->setPosition({500, 500, 500}).setScale({5, 5, 5});
}

void updateFPSLabel(UI &ui, const FPSCounter &fpsCounter) {
  auto [available, _] = getGPUMemoryUsage();
  ui.fpsLbl->text.setText(
      WString(L"FPS: " + std::to_wstring(fpsCounter.current()) + L" Avg: " +
              std::to_wstring(fpsCounter.average()) + L" memory unused: " +
              std::to_wstring(available)));
}

void main_marching_cubes(int argc, char *argv[]) {
  loc_assert(argc != 1, "Provide path for config");
  Conf config{argv[1]};
  // create window
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

  sdl2cpp::ui::UIManager uiManager{window, String{assetPath + "/gui/fonts"}};

  UI ui = initUI(uiManager);

  auto drawMode = DrawMode::Polygon;
  ui.lineFillBtn->setMouseClicked(
      [&drawMode, &ui](sdl2cpp::ui::EventInfo info,
                       sdl2cpp::ui::MouseButton button, SDL_Point pos) {
        static auto line = true;
        if (line) {
          drawMode = DrawMode::Line;
          ui.lineFillBtn->text.setText(L"Fill"_sw);
        } else {
          drawMode = DrawMode::Polygon;
          ui.lineFillBtn->text.setText(L"Line"_sw);
        }
        line = !line;
      });
  FPSCounter fpsCounter;

  ChunkManager chunks{ui.cameraController, config};
  chunks.surr.info.subscribe(
      [&ui](auto &val) { ui.chunkInfoLbl->text.setText(val); });

  SM sm;
  chunks.smProgram = sm.renderProgram;
  // chunks.texture = sm.depthMap;
  sm.lightPos = chunks.light.position;

  ModelRenderer modelRenderer;
  initModels(modelRenderer, assetPath);

  ge::gl::glEnable(GL_DEPTH_TEST);

  mainLoop->setIdleCallback([&]() {
    ge::gl::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    fpsCounter.frame();
    updateFPSLabel(ui, fpsCounter);

    chunks.generateChunks();

    chunks.render = false;
    sm.begin();
    chunks.draw(
        drawMode,
        {config.get<bool>("debug", "drawChunkBorder", "enabled").value(),
         config.get<bool>("debug", "drawNormals").value(),
         config.get<uint>("debug", "drawChunkBorder", "step").value()});

    modelRenderer.render(sm.program, ui.cameraController->getViewMatrix(),
                         false);
    sm.end();

    chunks.render = true;

    chunks.draw(
        drawMode,
        {config.get<bool>("debug", "drawChunkBorder", "enabled").value(),
         config.get<bool>("debug", "drawNormals").value(),
         config.get<uint>("debug", "drawChunkBorder", "step").value()});

    modelRenderer.render(sm.renderProgram, ui.cameraController->getViewMatrix(),
                         true);

    auto ortho = glm::ortho<float>(0, 1000, 0, 562.5, -1, 1);
    uiManager.render(ortho);

    window->swap();
  });

  (*mainLoop)();
}

Tmp::Tmp() {
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

SM::SM() {
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
  /* ge::gl::glTextureImage2DEXT(depthMap->getId(), GL_TEXTURE_2D, 0,
                               GL_DEPTH_COMPONENT, width, height, 0,
                               GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
   depthMap->texParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   depthMap->texParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   depthMap->texParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
   depthMap->texParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);

   fbo.attachTexture(GL_DEPTH_ATTACHMENT, depthMap);
   fbo.drawBuffer(GL_NONE);
   ge::gl::glNamedFramebufferReadBuffer(fbo.getId(), GL_NONE);*/
}
void SM::begin() {
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
void SM::end() {
  // fbo.unbind(GL_FRAMEBUFFER);
  ge::gl::glBindFramebuffer(GL_FRAMEBUFFER, 0);
  auto [x, y, width, height] = m_viewport;
  ge::gl::glViewport(x, y, width, height);
}
