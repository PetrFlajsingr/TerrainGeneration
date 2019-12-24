//
// Created by petr on 11/30/19.
//

#include "marching_cubes.h"
#include "Camera.h"
#include "rendering/Data.h"
#include "rendering/models/GraphicsModelBase.h"
#include "rendering/shadow_maps/CascadedShadowMap.h"
#include "rendering/utils/DrawTexture.h"
#include "ui/bindings/Binding.h"
#include "ui/elements.h"
#include "ui/managers.h"
#include "utils/config/JsonConfig.h"
#include "various/loc_assert.h"
#include <Colors.h>
#include <SDL2CPP/MainLoop.h>
#include <SDL2CPP/Window.h>
#include <graphics/gl_utils.h>
#include <rendering/marching_cubes/ChunkManager.h>
#include <rendering/models/ModelRenderer.h>
#include <time/FPSCounter.h>
#include <types.h>

using namespace sdl2cpp::ui;
using Conf = JsonConfig<true>;

struct UI {
  std::shared_ptr<Button> lineFillBtn;
  std::shared_ptr<Button> pauseMCBtn;
  std::shared_ptr<Label> fpsLbl;
  std::shared_ptr<Label> chunkInfoLbl;
  std::shared_ptr<Label> speedLbl;
  std::shared_ptr<CameraController> cameraController;
  std::shared_ptr<Slider<float>> movementSpeedSlider;
};

UI initUI(UIManager &uiManager) {
  printT(LogLevel::Info, "Initialising UI");
  auto perspective = PerspectiveProjection(0.1f, 1000000.f, 1920.f / 1080, glm::degrees(60.f));
  auto cameraController =
      uiManager.createGUIObject<CameraController>(std::move(perspective), glm::vec3{0, 0, 0}, glm::vec3{1920, 1080, 0});

  auto lineFillBtn = uiManager.createGUIObject<sdl2cpp::ui::Button>(glm::vec3{0, 0, 1}, glm::vec3{250, 100, 0});
  lineFillBtn->text.setFont("arialbd", 60);
  lineFillBtn->text.setText(L"Line"_sw);

  auto fpsLbl = uiManager.createGUIObject<Label>(glm::vec3{1300, 0, 1}, glm::vec3{220, 20, 0});
  fpsLbl->text.setFont("arialbd", 10);

  auto pauseMCBtn = uiManager.createGUIObject<Button>(glm::vec3{1700, 300, 1}, glm::vec3{220, 20, 0});
  pauseMCBtn->text.setFont("arialbd", 10);
  pauseMCBtn->text.setText(L"MC on/off"_sw);

  auto speedLbl = uiManager.createGUIObject<Label>(glm::vec3{0, 620, 1}, glm::vec3{220, 50, 0});
  speedLbl->text.setFont("arialbd", 10);
  speedLbl->text.setColor(Color::white);

  auto chunkInfoLbl = uiManager.createGUIObject<Label>(glm::vec3{0, 1000, 1}, glm::vec3{500, 20, 0});
  chunkInfoLbl->text.setFont("arialbd", 10);
  chunkInfoLbl->text.setColor({1, 1, 1, 1});

  auto movementSpeedSlider = uiManager.createGUIObject<Slider<float>>(glm::vec3{0, 700, 1}, glm::vec3{200, 50, 0});
  movementSpeedSlider->setColor(Color::transparent(Color::red, 0.5f));

  return {lineFillBtn, pauseMCBtn, fpsLbl, chunkInfoLbl, speedLbl, cameraController, movementSpeedSlider};
}

void initModels(ModelRenderer &modelRenderer, const std::string &assetPath) {}

void updateFPSLabel(UI &ui, const FPSCounter &fpsCounter) {
  auto [available, _] = getGPUMemoryUsage();
  ui.fpsLbl->text.setText(WString(L"FPS: " + std::to_wstring(fpsCounter.current()) + L" Avg: " +
                                  std::to_wstring(fpsCounter.average()) + L" memory unused: " + std::to_wstring(available)));
}

void main_marching_cubes(int argc, char *argv[]) {
  loc_assert(argc != 1, "Provide path for config");
  printT(LogLevel::Info, "Loading config");
  Conf config{argv[1]};
  auto mainLoop = std::make_shared<sdl2cpp::MainLoop>();

  const auto deviceData = config.get<DeviceData>("device").value();
  auto window = std::make_shared<sdl2cpp::Window>(deviceData.screen.width, deviceData.screen.height);
  window->createContext("rendering", 430);
  mainLoop->addWindow("mainWindow", window);

  printT(LogLevel::Status, "Initialising OpenGL");
  ge::gl::init(SDL_GL_GetProcAddress);
  ge::gl::setHighDebugMessage();

  ge::gl::glClearColor(0, 0, 0, 1);

  setShaderLocation(config.get<std::string>("paths", "shaderLocation").value());

  const auto assetPath = config.get<std::string>("paths", "assetsLocation").value();

  sdl2cpp::ui::UIManager uiManager{window, String{assetPath + "/gui/fonts"}};

  UI ui = initUI(uiManager);

  auto drawMode = DrawMode::Polygon;
  ui.lineFillBtn->setMouseClicked([&drawMode, &ui](sdl2cpp::ui::EventInfo info, sdl2cpp::ui::MouseButton button, SDL_Point pos) {
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

  ui.movementSpeedSlider->setMin(1.0f).setMax(2000.0f).setSliderValue(10.0f);
  ui.movementSpeedSlider->value.subscribe_and_call([&ui](auto sliderValue) {
    ui.cameraController->setMovementSpeed(sliderValue);
    ui.speedLbl->text.setText(L"Current speed: {}"_sw.format(sliderValue));
  });

  FPSCounter fpsCounter;

  printT(LogLevel::Status, "Initialising chunk manager");
  ChunkManager chunks{ui.cameraController, config};
  chunks.surr.info.subscribe([&ui](auto &val) { ui.chunkInfoLbl->text.setText(val); });

  printT(LogLevel::Status, "Initialising models");
  ModelRenderer modelRenderer;
  initModels(modelRenderer, assetPath);

  auto renderProgram = std::make_shared<ge::gl::Program>("shadow_map/cascade_render"_vert, "shadow_map/cascade_render"_frag);

  CascadedShadowMap cascadedShadowMap{4, 4096};
  cascadedShadowMap.setLightDir({0.1, -0.9, 0});

  chunks.smProgram = renderProgram;

  bool showTextures = false;
  auto textureBtn = uiManager.createGUIObject<sdl2cpp::ui::Button>(glm::vec3{0, 100, 1}, glm::vec3{250, 100, 0});
  textureBtn->setMouseClicked([&showTextures] { showTextures = !showTextures; });

  bool pauseMC = false;
  ui.pauseMCBtn->setMouseClicked([&pauseMC] {
    pauseMC = !pauseMC;
  });

  DrawTexture drawTexture;

  ge::gl::glEnable(GL_DEPTH_TEST);

  printT(LogLevel::Status, "All set, starting main loop");
  mainLoop->setIdleCallback([&]() {
    ge::gl::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    fpsCounter.frame();
    updateFPSLabel(ui, fpsCounter);

    if (!pauseMC) {
      chunks.generateChunks();
    }

    chunks.render = false;
    auto renderFnc = [&ui, &chunks, &modelRenderer](const auto &program, const auto &aabb) {
      glm::mat4 model{1.f};
      program->setMatrix4fv("model", &model[0][0]);
      chunks.drawToShadowMap(aabb);
      modelRenderer.render(program, ui.cameraController->getViewMatrix(), false);
    };
    cascadedShadowMap.renderShadowMap(renderFnc, ui.cameraController->camera.projection, ui.cameraController->getViewMatrix());

    if (showTextures) {
      drawTexture.drawCasc(cascadedShadowMap.getDepthMap());
    } else {
      chunks.render = true;

      renderProgram->use();
      cascadedShadowMap.bindRender(renderProgram);
      renderProgram->setMatrix4fv("inverseViewMatrix", glm::value_ptr(glm::inverse(ui.cameraController->getViewMatrix())));

      renderProgram->set3fv("lightDir", glm::value_ptr(cascadedShadowMap.getLightDir()));
      renderProgram->setMatrix4fv("projection", glm::value_ptr(ui.cameraController->camera.projection.matrix.getRef()));
      ge::gl::glUniform1i(renderProgram->getUniformLocation("cascadedDepthTexture"), 0);

      chunks.draw(drawMode, {config.get<bool>("debug", "drawChunkBorder", "enabled").value(),
                             config.get<bool>("debug", "drawNormals").value(),
                             config.get<unsigned int>("debug", "drawChunkBorder", "step").value()});

      modelRenderer.render(renderProgram, ui.cameraController->getViewMatrix(), true);
    }
    auto ortho = glm::ortho<float>(0, 1000, 0, 562.5, -1, 1);
    uiManager.render(ortho);

    window->swap();
  });

  (*mainLoop)();
}
