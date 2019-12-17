//
// Created by petr on 11/30/19.
//

#include "marching_cubes.h"
#include "rendering/Data.h"
#include "rendering/shading/CascadedShadowMap.h"
#include "rendering/utils/DrawTexture.h"
#include "ui/elements.h"
#include "ui/managers.h"
#include "utils/config/JsonConfig.h"
#include "various/loc_assert.h"
#include <SDL2CPP/MainLoop.h>
#include <SDL2CPP/Window.h>
#include <gl_utils.h>
#include <rendering/marching_cubes/ChunkManager.h>
#include <rendering/models/ModelRenderer.h>
#include <time/FPSCounter.h>
#include <types.h>
#include "rendering/models/GraphicsModelBase.h"

using namespace sdl2cpp::ui;
using Conf = JsonConfig<true>;

struct UI {
  std::shared_ptr<Button> lineFillBtn;
  std::shared_ptr<Label> fpsLbl;
  std::shared_ptr<Label> chunkInfoLbl;
  std::shared_ptr<CameraController> cameraController;
};

UI initUI(UIManager &uiManager) {
  auto perspective = PerspectiveProjection(0.1f, 500.f,  1920.f / 1080, 60.f);
  auto cameraController = uiManager.createGUIObject<CameraController>(perspective,
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
  ObjModelLoader loader(assetPath + "/models");
  for (int i : range(27)) {
    modelRenderer.addModel(loader.loadModel("bah.obj", "uga" + std::to_string(i)));
  }
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

  ModelRenderer modelRenderer;
  initModels(modelRenderer, assetPath);

  std::vector<std::shared_ptr<GraphicsModelBase>> ugas;
  for (int i : range(27)) {
    ugas.emplace_back(modelRenderer.modelById("uga" + std::to_string(i)).value());
  }
  auto renderProgram = std::make_shared<ge::gl::Program>(
      "shadow_map/cascade_render"_vert, "shadow_map/cascade_render"_frag);

  CascadedShadowMap cascadedShadowMap{4, 4096};
  cascadedShadowMap.setLightDir({0.5, -0.5, 0});

  chunks.smProgram = renderProgram;

  bool showTextures = false;
  auto textureBtn = uiManager.createGUIObject<sdl2cpp::ui::Button>(
      glm::vec3{0, 100, 1}, glm::vec3{250, 100, 0});
  textureBtn->setMouseClicked(
      [&showTextures] { showTextures = !showTextures; });
  DrawTexture drawTexture;

  ge::gl::glEnable(GL_DEPTH_TEST);

  mainLoop->setIdleCallback([&]() {
    ge::gl::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    fpsCounter.frame();
    updateFPSLabel(ui, fpsCounter);

    unsigned int cnt = 0;
    for (auto &model : ugas) {
      model->setPosition(chunks.surr.partsMap[static_cast<SurrPos>(cnt)]->center);
      ++cnt;
    }

    chunks.generateChunks();

    chunks.render = false;
    auto renderFnc = [&ui, &chunks, &modelRenderer](const auto &program,
                                                    const auto &aabb) {
      glm::mat4 model{1.f};
      program->setMatrix4fv("model", &model[0][0]);
      chunks.drawToShadowMap(aabb);
      modelRenderer.render(program, ui.cameraController->getViewMatrix(),
                           false);
    };
    cascadedShadowMap.renderShadowMap(renderFnc, ui.cameraController->camera.projection.matrix,
                                      ui.cameraController->getViewMatrix(),
                                      0.1f, 500.f, 1920.f / 1080, 60.f);

    if (showTextures) {
      drawTexture.drawCasc(cascadedShadowMap.getDepthMap());
    } else {
      chunks.render = true;

      renderProgram->use();
      cascadedShadowMap.bindRender(renderProgram);
      renderProgram->setMatrix4fv(
          "inverseViewMatrix",
          glm::value_ptr(glm::inverse(ui.cameraController->getViewMatrix())));

      renderProgram->set3fv("lightDir",
                            glm::value_ptr(cascadedShadowMap.getLightDir()));
      renderProgram->setMatrix4fv("projection", glm::value_ptr(ui.cameraController->camera.projection.matrix));
      ge::gl::glUniform1i(
          renderProgram->getUniformLocation("cascadedDepthTexture"), 0);

      chunks.draw(
          drawMode,
          {config.get<bool>("debug", "drawChunkBorder", "enabled").value(),
           config.get<bool>("debug", "drawNormals").value(),
           config.get<unsigned int>("debug", "drawChunkBorder", "step").value()});

      modelRenderer.render(renderProgram, ui.cameraController->getViewMatrix(),
                           true);
    }
    auto ortho = glm::ortho<float>(0, 1000, 0, 562.5, -1, 1);
    uiManager.render(ortho);

    window->swap();
  });

  (*mainLoop)();
}
