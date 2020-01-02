//
// Created by petr on 11/30/19.
//
#define STB_IMAGE_IMPLEMENTATION
#include "marching_cubes.h"
#include "Camera.h"
#include "MarchingCubesUI.h"
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

  UI ui{uiManager};

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

  CascadedShadowMap cascadedShadowMap{4, 1024};
  cascadedShadowMap.setLightDir({0.9, -0.9, 0});

  chunks.smProgram = renderProgram;

  bool showTextures = false;
  ui.shadowMapsBtn->setMouseClicked([&showTextures] { showTextures = !showTextures; });

  bool pauseMC = false;
  ui.pauseMCBtn->setMouseClicked([&pauseMC] { pauseMC = !pauseMC; });

  DrawTexture drawTexture;

  ge::gl::glEnable(GL_DEPTH_TEST);

  ObjModelLoader loader{assetPath + "/models/"};
  EnvironmentRenderer envRenderer{loader};
  envRenderer.setCloudRelativePosition(20000);
  envRenderer.setWaterLevel(8500);

  ui.uiSwitch->isOn.subscribe_and_call([&ui](const auto &value) { ui.setVisible(value); });

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

  struct {
    unsigned int tex2;
    unsigned int tex1;
    unsigned int tex3;
  } sandTex;

  sandTex.tex2 = textureLoader.loadTexture("sand/sand_ripples.jpg", texOptions);
  sandTex.tex1 = textureLoader.loadTexture("Seamless ground rock.jpg", texOptions);
  sandTex.tex3 = textureLoader.loadTexture("desert sand.jpg", texOptions);

  struct {
    unsigned int tex2;
    unsigned int tex1;
    unsigned int tex3;
  } grassTex;

  grassTex.tex2 = textureLoader.loadTexture("Grass Green Textures/GrassGreenTexture0003.jpg", texOptions);
  grassTex.tex1 = textureLoader.loadTexture("Seamless ground rock.jpg", texOptions);
  grassTex.tex3 = textureLoader.loadTexture("desert sand.jpg", texOptions);

  using namespace std::chrono_literals;
  float time = 0;

  printT(LogLevel::Status, "All set, starting main loop");

  bool useGrassTex = true;

  auto texBtn = uiManager.createGUIObject<Button>(glm::vec3{550, 0, 1}, glm::vec3{100, 50, 0});
  texBtn->setMouseClicked([&useGrassTex] { useGrassTex = !useGrassTex; });

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

      if (useGrassTex) {
        ge::gl::glActiveTexture(GL_TEXTURE0 + 1);
        ge::gl::glBindTexture(GL_TEXTURE_2D, grassTex.tex1);
        ge::gl::glActiveTexture(GL_TEXTURE0 + 2);
        ge::gl::glBindTexture(GL_TEXTURE_2D, grassTex.tex2);
        ge::gl::glActiveTexture(GL_TEXTURE0 + 3);
        ge::gl::glBindTexture(GL_TEXTURE_2D, grassTex.tex3);
      } else {
        {
          ge::gl::glActiveTexture(GL_TEXTURE0 + 1);
          ge::gl::glBindTexture(GL_TEXTURE_2D, sandTex.tex1);
          ge::gl::glActiveTexture(GL_TEXTURE0 + 2);
          ge::gl::glBindTexture(GL_TEXTURE_2D, sandTex.tex2);
          ge::gl::glActiveTexture(GL_TEXTURE0 + 3);
          ge::gl::glBindTexture(GL_TEXTURE_2D, sandTex.tex3);
        }
      }

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
