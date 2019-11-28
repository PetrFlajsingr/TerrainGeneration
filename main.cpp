#include "freetype-gl++/freetype-gl++.hpp"

#include <geGL/StaticCalls.h>
#include <geGL/geGL.h>

#include "io/print.h"
#include "time/FPSCounter.h"
#include <types.h>

#include "rendering/ChunkManager.h"
#include "rendering/Data.h"
#include "ui/elements/Button.h"
#include "ui/elements/CameraController.h"
#include "ui/elements/KeyAction.h"
#include "ui/managers/GUIManager.h"
#include "utils/config/JsonConfig.h"

#include <SDL2CPP/MainLoop.h>
#include <SDL2CPP/Window.h>
#include <ui/elements/Label.h>

using namespace std::string_literals;
using namespace ShaderLiterals;
using namespace sdl2cpp::ui;

using Conf = JsonConfig<true>;

int main(int argc, char *argv[]) {
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

  sdl2cpp::ui::GUIManager guiManager{window, String{assetPath + "/gui/fonts"}};

  auto cameraController = guiManager.createGUIObject<CameraController>(
      glm::vec3{0, 0, 0}, glm::vec3{1920, 1080, 0});

  auto testAction =
      guiManager.createGUIObject<KeyAction>(SDLK_r, [] { print("test mman"); });

  auto testBtn = guiManager.createGUIObject<sdl2cpp::ui::Button>(
      glm::vec3{0, 0, 1}, glm::vec3{250, 100, 0});
  testBtn->text.setFont("arialbd", 60);
  testBtn->text.setText(L"Line"_sw);

  auto drawMode = DrawMode::Polygon;
  testBtn->setMouseClicked(
      [&drawMode, &testBtn](sdl2cpp::ui::EventInfo info,
                            sdl2cpp::ui::MouseButton button, SDL_Point pos) {
        static auto line = true;
        if (line) {
          drawMode = DrawMode::Line;
          testBtn->text.setText(L"Fill"_sw);
        } else {
          drawMode = DrawMode::Polygon;
          testBtn->text.setText(L"Line"_sw);
        }
        line = !line;
      });

  auto fpsLbl = guiManager.createGUIObject<Label>(glm::vec3{1300, 0, 1},
                                                  glm::vec3{220, 20, 0});
  fpsLbl->text.setFont("arialbd", 10);

  auto chunkInfoLbl = guiManager.createGUIObject<Label>(glm::vec3{0, 1000, 1},
                                                        glm::vec3{500, 20, 0});
  chunkInfoLbl->text.setFont("arialbd", 10);
  chunkInfoLbl->text.setColor({1, 1, 1, 1});

  FPSCounter fpsCounter;

  ChunkManager chunks{cameraController, config};
  chunks.surr.info.subscribe(
      [&chunkInfoLbl](auto &val) { chunkInfoLbl->text.setText(val); });


  mainLoop->setIdleCallback([&]() {
    ge::gl::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049
    GLint total_mem_kb = 0;
    ge::gl::glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX,
                          &total_mem_kb);

    GLint cur_avail_mem_kb = 0;
    ge::gl::glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX,
                          &cur_avail_mem_kb);

    fpsCounter.frame();
    fpsLbl->text.setText(
        WString(L"FPS: " + std::to_wstring(fpsCounter.current()) + L" Avg: " +
                std::to_wstring(fpsCounter.average()) + L" memory unused: " +
                std::to_wstring(cur_avail_mem_kb)));

    chunks.generateChunks();
    chunks.draw(
        drawMode,
        {config.get<bool>("debug", "drawChunkBorder", "enabled").value(),
         config.get<bool>("debug", "drawNormals").value(),
         config.get<uint>("debug", "drawChunkBorder", "step").value()});

    auto ortho = glm::ortho<float>(0, 1000, 0, 562.5, -1, 1);
    guiManager.render(ortho);

    window->swap();
  });

  (*mainLoop)();

  return 0;
}
