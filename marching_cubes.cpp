//
// Created by petr on 11/30/19.
//
#define STB_IMAGE_IMPLEMENTATION
#include "marching_cubes.h"
#include "MarchingCubesUI.h"
#include "common/ConfigData.h"
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

using namespace sdl2cpp::ui;
using Conf = JsonConfig<true>;

void updateFPSLabel(UI &ui, const FPSCounter &fpsCounter) {
  auto [available, _] = getGPUMemoryUsage();
  ui.fpsLbl->text.setText(WString(L"FPS: " + std::to_wstring(fpsCounter.current()) + L" Avg: " +
                                  std::to_wstring(fpsCounter.average()) + L" memory unused: " + std::to_wstring(available)));
}

std::pair<unsigned int, unsigned int> getDisplaySize() {
  SDL_DisplayMode DM;
  if (SDL_GetDesktopDisplayMode(0, &DM) != 0) {
    throw exc::Error("SDL_GetDesktopDisplayMode failed");
  }
  unsigned int w = DM.w * 0.8;
  unsigned int h = DM.h * 0.8;
  return {w, h};
}

void main_marching_cubes(int argc, char *argv[]) {
  loc_assert(argc != 1, "Provide path for config");
  printT(LogLevel::Info, "Loading config");
  Conf config{argv[1]};

  const auto configData = config.get<ConfigData>().value();
  auto mainLoop = std::make_shared<sdl2cpp::MainLoop>();

  const auto [screenWidth, screenHeight] = getDisplaySize();
  auto window = std::make_shared<sdl2cpp::Window>(screenWidth, screenHeight);
  window->createContext("rendering", 430);
  mainLoop->addWindow("mainWindow", window);

  printT(LogLevel::Status, "Initialising OpenGL");
  ge::gl::init(SDL_GL_GetProcAddress);
  ge::gl::setHighDebugMessage();

  ge::gl::glClearColor(0, 153.0f / 255, 203.0f / 255, 1);

  setShaderLocation(configData.paths.shaderLocation);

  const auto assetPath = configData.paths.assetsLocation;

  sdl2cpp::ui::UIManager uiManager{window, String{assetPath + "/gui/fonts"}};

  UI ui{uiManager};

  auto drawMode = DrawMode::Polygon;
  ui.lineFillBtn->setMouseClicked([&drawMode, &ui] {
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

  ui.movementSpeedSlider->value.subscribe_and_call([&ui](auto sliderValue) {
    ui.cameraController->setMovementSpeed(sliderValue);
    ui.speedLbl->text.setText(L"Current speed: {}"_sw.format(sliderValue));
  });

  FPSCounter fpsCounter;

  printT(LogLevel::Status, "Initialising chunk manager");
  ChunkManager chunks{ui.cameraController, configData};
  chunks.getSurroundings().info.subscribe([&ui](auto &val) { ui.chunkInfoLbl->text.setText(val); });

  printT(LogLevel::Status, "Initialising models");
  ModelRenderer modelRenderer;

  auto renderProgram = std::make_shared<ge::gl::Program>("shadow_map/cascade_render"_vert, "shadow_map/cascade_render"_frag);

  CascadedShadowMap cascadedShadowMap{4, 4096};
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

  ui.terrain.applyChangesBtn->setMouseClicked([&ui, &chunks] {
    chunks.getGenerationOptions()
        .setOctaves(ui.terrain.octavesSlider->value.get())
        .setGain(ui.terrain.gainSlider->value.get())
        .setLacunarity(ui.terrain.lacunaritySlider->value.get())
        .setSharpness(ui.terrain.sharpnessSlider->value.get())
        .setValleyScale(ui.terrain.valleyScaleSlider->value.get())
        .setHeightScale(ui.terrain.heightScaleSlider->value.get());
    chunks.invalidate();
    chunks.getSurroundings().info.subscribe([&ui](auto &val) { ui.chunkInfoLbl->text.setText(val); });
  });
  ui.terrain.randomizeBtn->setMouseClicked([&ui, &chunks] {
    chunks.getGenerationOptions().randomize();
    ui.terrain.octavesSlider->setSliderValue(chunks.getGenerationOptions().getOctaves());
    ui.terrain.gainSlider->setSliderValue(chunks.getGenerationOptions().getGain());
    ui.terrain.lacunaritySlider->setSliderValue(chunks.getGenerationOptions().getLacunarity());
    ui.terrain.sharpnessSlider->setSliderValue(chunks.getGenerationOptions().getSharpness());
    ui.terrain.valleyScaleSlider->setSliderValue(chunks.getGenerationOptions().getValleyScale());
    ui.terrain.heightScaleSlider->setSliderValue(chunks.getGenerationOptions().getHeightScale());
    chunks.invalidate();
    chunks.getSurroundings().info.subscribe([&ui](auto &val) { ui.chunkInfoLbl->text.setText(val); });
  });

  ui.lightDirSlider->value.subscribe_and_call([&cascadedShadowMap] (float value) {
    const auto y = 1.0f - glm::abs(value);
    print(glm::vec3{1.f - y, -y, 0});
    if (value < 0) {
      cascadedShadowMap.setLightDir({-(1.f - y), -y, 0});
    } else {
      cascadedShadowMap.setLightDir({1.f - y, -y, 0});
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

  bool useGrassTex = true;
  auto texBtn = uiManager.createGUIObject<Button>(glm::vec3{550, 0, 1}, glm::vec3{100, 50, 0});
  texBtn->setMouseClicked([&useGrassTex] { useGrassTex = !useGrassTex; });

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

      chunks.draw(drawMode);

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
