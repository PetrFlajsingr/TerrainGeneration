//
// Created by petr on 11/30/19.
//
#define STB_IMAGE_IMPLEMENTATION
#include "marching_cubes.h"
#include "Camera.h"
#include "rendering/Data.h"
#include "rendering/shadow_maps/CascadedShadowMap.h"
#include "rendering/textures/DrawTexture.h"
#include "rendering/textures/FileTextureLoader.h"
#include "ui/bindings/Binding.h"
#include "ui/elements.h"
#include "ui/managers.h"
#include "utils/config/JsonConfig.h"
#include "various/loc_assert.h"
#include <Colors.h>
#include <SDL2CPP/MainLoop.h>
#include <SDL2CPP/Window.h>
#include <graphics/gl_utils.h>
#include <rendering/environment/EnvironmentRenderer.h>
#include <rendering/marching_cubes/ChunkManager.h>
#include <rendering/models/ModelRenderer.h>
#include <time/FPSCounter.h>
#include <types.h>
#include <ui/elements/Switch.h>

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
  std::shared_ptr<Switch> uiSwitch;

  struct {
    std::shared_ptr<Label> octavesLbl;
    std::shared_ptr<Label> gainLbl;
    std::shared_ptr<Label> lacunarityLbl;
    std::shared_ptr<Label> sharpnessLbl;
    std::shared_ptr<Label> valleyScaleLbl;
    std::shared_ptr<Label> heightScaleLbl;

    std::shared_ptr<Label> octavesValLbl;
    std::shared_ptr<Label> gainValLbl;
    std::shared_ptr<Label> lacunarityValLbl;
    std::shared_ptr<Label> sharpnessValLbl;
    std::shared_ptr<Label> valleyScaleValLbl;
    std::shared_ptr<Label> heightScaleValLbl;

    std::shared_ptr<Slider<unsigned int>> octavesSlider;
    std::shared_ptr<Slider<float>> gainSlider;
    std::shared_ptr<Slider<float>> lacunaritySlider;
    std::shared_ptr<Slider<float>> sharpnessSlider;
    std::shared_ptr<Slider<float>> valleyScaleSlider;
    std::shared_ptr<Slider<float>> heightScaleSlider;
  } terrain;
};

UI initUI(UIManager &uiManager) {
  printT(LogLevel::Info, "Initialising UI");
  auto perspective = PerspectiveProjection(0.1f, 500000.f, 1920.f / 1080, glm::degrees(60.f));
  auto cameraController =
      uiManager.createGUIObject<CameraController>(std::move(perspective), glm::vec3{0, 0, 0}, glm::vec3{1920, 1080, 0});

  auto lineFillBtn = uiManager.createGUIObject<sdl2cpp::ui::Button>(glm::vec3{0, 0, 1}, glm::vec3{150, 60, 0});
  lineFillBtn->text.setFont("arialbd", 40).setText(L"Line"_sw);

  auto fpsLbl = uiManager.createGUIObject<Label>(glm::vec3{1300, 0, 1}, glm::vec3{220, 20, 0});
  fpsLbl->text.setFont("arialbd", 10);

  auto pauseMCBtn = uiManager.createGUIObject<Button>(glm::vec3{1700, 20, 1}, glm::vec3{150, 20, 0});
  pauseMCBtn->text.setFont("arialbd", 10).setText(L"MC on/off"_sw);

  auto speedLbl = uiManager.createGUIObject<Label>(glm::vec3{0, 620, 1}, glm::vec3{220, 50, 0});
  speedLbl->text.setFont("arialbd", 10).setColor(Color::white);
  auto movementSpeedSlider = uiManager.createGUIObject<Slider<float>>(glm::vec3{0, 700, 1}, glm::vec3{200, 50, 0});
  movementSpeedSlider->setColor(Color::transparent(Color::red, 0.5f));

  auto chunkInfoLbl = uiManager.createGUIObject<Label>(glm::vec3{0, 1000, 1}, glm::vec3{500, 20, 0});
  chunkInfoLbl->text.setFont("arialbd", 10).setColor(Color::white);

  auto octavesLbl = uiManager.createGUIObject<Label>(glm::vec3{1400, 350 + 500, 1}, glm::vec3{70, 30, 0});
  octavesLbl->text.setFont("arialbd", 10).setColor(Color::white).setText(L"Octaves:"_sw);
  auto gainLbl = uiManager.createGUIObject<Label>(glm::vec3{1400, 350 + 530, 1}, glm::vec3{70, 30, 0});
  gainLbl->text.setFont("arialbd", 10).setColor(Color::white).setText(L"Gain:"_sw);
  auto lacunarityLbl = uiManager.createGUIObject<Label>(glm::vec3{1400, 350 + 560, 1}, glm::vec3{70, 30, 0});
  lacunarityLbl->text.setFont("arialbd", 10).setColor(Color::white).setText(L"Lacunarity:"_sw);
  auto sharpnessLbl = uiManager.createGUIObject<Label>(glm::vec3{1400, 350 + 590, 1}, glm::vec3{70, 30, 0});
  sharpnessLbl->text.setFont("arialbd", 10).setColor(Color::white).setText(L"Sharpness:"_sw);
  auto valleyScaleLbl = uiManager.createGUIObject<Label>(glm::vec3{1400, 350 + 620, 1}, glm::vec3{70, 30, 0});
  valleyScaleLbl->text.setFont("arialbd", 10).setColor(Color::white).setText(L"Valley scale:"_sw);
  auto heightScaleLbl = uiManager.createGUIObject<Label>(glm::vec3{1400, 350 + 650, 1}, glm::vec3{70, 30, 0});
  heightScaleLbl->text.setFont("arialbd", 10).setColor(Color::white).setText(L"Height scale:"_sw);

  auto octavesValLbl = uiManager.createGUIObject<Label>(glm::vec3{1840, 350 + 500, 1}, glm::vec3{100, 30, 0});
  octavesValLbl->text.setFont("arialbd", 10).setColor(Color::white);
  auto gainValLbl = uiManager.createGUIObject<Label>(glm::vec3{1840, 350 + 530, 1}, glm::vec3{100, 30, 0});
  gainValLbl->text.setFont("arialbd", 10).setColor(Color::white);
  auto lacunarityValLbl = uiManager.createGUIObject<Label>(glm::vec3{1840, 350 + 560, 1}, glm::vec3{100, 30, 0});
  lacunarityValLbl->text.setFont("arialbd", 10).setColor(Color::white);
  auto sharpnessValLbl = uiManager.createGUIObject<Label>(glm::vec3{1840, 350 + 590, 1}, glm::vec3{100, 30, 0});
  sharpnessValLbl->text.setFont("arialbd", 10).setColor(Color::white);
  auto valleyScaleValLbl = uiManager.createGUIObject<Label>(glm::vec3{1840, 350 + 620, 1}, glm::vec3{100, 30, 0});
  valleyScaleValLbl->text.setFont("arialbd", 10).setColor(Color::white);
  auto heightScaleValLbl = uiManager.createGUIObject<Label>(glm::vec3{1840, 350 + 650, 1}, glm::vec3{100, 30, 0});
  heightScaleValLbl->text.setFont("arialbd", 10).setColor(Color::white);

  auto octavesSlider = uiManager.createGUIObject<Slider<unsigned int>>(glm::vec3{1520, 365 + 500, 1}, glm::vec3{315, 20, 0});
  octavesSlider->value.subscribe(
      [octavesValLbl](const auto &value) { octavesValLbl->text.setText(WString{std::to_wstring(value)}); });
  octavesSlider->setMin(1).setMax(10).setSliderValue(4);
  auto gainSlider = uiManager.createGUIObject<Slider<float>>(glm::vec3{1520, 365 + 530, 1}, glm::vec3{315, 20, 0});
  gainSlider->value.subscribe(
      [gainValLbl](const auto &value) { gainValLbl->text.setText(WString{std::to_wstring(value)}); });
  gainSlider->setMin(0.1).setMax(100000).setSliderValue(30000);
  auto lacunaritySlider = uiManager.createGUIObject<Slider<float>>(glm::vec3{1520, 365 + 560, 1}, glm::vec3{315, 20, 0});
  lacunaritySlider->value.subscribe(
      [lacunarityValLbl](const auto &value) { lacunarityValLbl->text.setText(WString{std::to_wstring(value)}); });
  lacunaritySlider->setMin(0.1).setMax(1000).setSliderValue(2);
  auto sharpnessSlider = uiManager.createGUIObject<Slider<float>>(glm::vec3{1520, 365 + 590, 1}, glm::vec3{315, 20, 0});
  sharpnessSlider->value.subscribe(
      [sharpnessValLbl](const auto &value) { sharpnessValLbl->text.setText(WString{std::to_wstring(value)}); });
  sharpnessSlider->setMin(-1).setMax(1).setSliderValue(0);
  auto valleyScaleSlider = uiManager.createGUIObject<Slider<float>>(glm::vec3{1520, 365 + 620, 1}, glm::vec3{315, 20, 0});
  valleyScaleSlider->value.subscribe(
      [valleyScaleValLbl](const auto &value) { valleyScaleValLbl->text.setText(WString{std::to_wstring(value)}); });
  valleyScaleSlider->setMin(0.00001).setMax(10).setSliderValue(0);
  auto heightScaleSlider = uiManager.createGUIObject<Slider<float>>(glm::vec3{1520, 365 + 650, 1}, glm::vec3{315, 20, 0});
  heightScaleSlider->value.subscribe(
      [heightScaleValLbl](const auto &value) { heightScaleValLbl->text.setText(WString{std::to_wstring(value)}); });
  heightScaleSlider->setMin(0).setMax(1000).setSliderValue(1);

  auto uiSwitch = uiManager.createGUIObject<Switch>(glm::vec3{50, 800, 1}, glm::vec3{50, 50, 0}, true);

  return {lineFillBtn,
          pauseMCBtn,
          fpsLbl,
          chunkInfoLbl,
          speedLbl,
          cameraController,
          movementSpeedSlider,
          uiSwitch,
          {
              octavesLbl, gainLbl, lacunarityLbl, sharpnessLbl, valleyScaleLbl, heightScaleLbl, octavesValLbl, gainValLbl,
              lacunarityValLbl, sharpnessValLbl, valleyScaleValLbl, heightScaleValLbl,
              octavesSlider,
              gainSlider,
              lacunaritySlider,
              sharpnessSlider,
              valleyScaleSlider,
              heightScaleSlider
          }};
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

  ge::gl::glClearColor(0, 153.0f / 255, 203.0f / 255, 1);

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

  ui.movementSpeedSlider->setMin(1.0f).setMax(5000.0f).setSliderValue(10.0f);
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
  cascadedShadowMap.setLightDir({0.9, -0.9, 0});

  chunks.smProgram = renderProgram;

  bool showTextures = false;
  auto textureBtn = uiManager.createGUIObject<sdl2cpp::ui::Button>(glm::vec3{0, 100, 1}, glm::vec3{250, 100, 0});
  textureBtn->setMouseClicked([&showTextures] { showTextures = !showTextures; });

  bool pauseMC = false;
  ui.pauseMCBtn->setMouseClicked([&pauseMC] { pauseMC = !pauseMC; });

  DrawTexture drawTexture;

  ge::gl::glEnable(GL_DEPTH_TEST);

  ObjModelLoader loader{assetPath + "/models/"};
  EnvironmentRenderer envRenderer{loader};
  envRenderer.setCloudRelativePosition(20000);
  envRenderer.setWaterLevel(8500);

  ui.uiSwitch->isOn.subscribe_and_call([&ui](const auto &value) {
    if (!value) {
      ui.lineFillBtn->setVisibility(Visibility::Invisible);
      ui.pauseMCBtn->setVisibility(Visibility::Invisible);
      ui.fpsLbl->setVisibility(Visibility::Invisible);
      ui.chunkInfoLbl->setVisibility(Visibility::Invisible);
      ui.speedLbl->setVisibility(Visibility::Invisible);
      ui.cameraController->setVisibility(Visibility::Invisible);
      ui.movementSpeedSlider->setVisibility(Visibility::Invisible);
    } else {
      ui.lineFillBtn->setVisibility(Visibility::Visible);
      ui.pauseMCBtn->setVisibility(Visibility::Visible);
      ui.fpsLbl->setVisibility(Visibility::Visible);
      ui.chunkInfoLbl->setVisibility(Visibility::Visible);
      ui.speedLbl->setVisibility(Visibility::Visible);
      ui.cameraController->setVisibility(Visibility::Visible);
      ui.movementSpeedSlider->setVisibility(Visibility::Visible);
    }
  });

  FileTextureLoader textureLoader{assetPath};
  TexOptions texOptions{GL_TEXTURE_2D,
                        GL_RGB,
                        true,
                        {
                            {GL_TEXTURE_WRAP_S, GL_REPEAT},
                            {GL_TEXTURE_WRAP_T, GL_REPEAT},
                            {GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR},
                            {GL_TEXTURE_MAG_FILTER, GL_LINEAR},
                        }};
  unsigned int tex2 = textureLoader.loadTexture("Grass Green Textures/GrassGreenTexture0003.jpg", texOptions);
  unsigned int tex1 = textureLoader.loadTexture("Seamless ground rock.jpg", texOptions);
  unsigned int tex3 = textureLoader.loadTexture("desert sand.jpg", texOptions);

  using namespace std::chrono_literals;
  float time = 0;

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
      ge::gl::glDisable(GL_BLEND);
      chunks.render = true;

      renderProgram->use();

      ge::gl::glActiveTexture(GL_TEXTURE0 + 1);
      ge::gl::glBindTexture(GL_TEXTURE_2D, tex1);
      ge::gl::glActiveTexture(GL_TEXTURE0 + 2);
      ge::gl::glBindTexture(GL_TEXTURE_2D, tex2);
      ge::gl::glActiveTexture(GL_TEXTURE0 + 3);
      ge::gl::glBindTexture(GL_TEXTURE_2D, tex3);

      ge::gl::glUniform1i(renderProgram->getUniformLocation("texturePlusX"), 3);
      ge::gl::glUniform1i(renderProgram->getUniformLocation("texturePlusY"), 2);
      ge::gl::glUniform1i(renderProgram->getUniformLocation("texturePlusZ"), 3);
      ge::gl::glUniform1i(renderProgram->getUniformLocation("textureMinusX"), 3);
      ge::gl::glUniform1i(renderProgram->getUniformLocation("textureMinusY"), 1);
      ge::gl::glUniform1i(renderProgram->getUniformLocation("textureMinusZ"), 3);
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
    envRenderer.render(*ui.cameraController, time);
    auto ortho = glm::ortho<float>(0, 1000, 0, 562.5, -1, 1);
    uiManager.render(ortho);

    window->swap();

    time += 0.008;
  });

  (*mainLoop)();
}
