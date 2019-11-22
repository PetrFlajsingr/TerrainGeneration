#include "freetype-gl++/freetype-gl++.hpp"
#include "freetype-gl++/texture-atlas+.hpp"
#include "freetype-gl++/vertex-buffer+.hpp"
#include <freetype-gl++/shader+.hpp>

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

using namespace std::string_literals;
using namespace ShaderLiterals;

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
  testBtn->text.setText(L"Line"_sw);

  auto drawMode = DrawMode::Polygon;
  testBtn->setMouseOut([](sdl2cpp::ui::EventInfo info) { print("MouseOut"); })
      .setMouseOver([](sdl2cpp::ui::EventInfo info) { print("MouseOver"); })
      .setMouseClicked([&drawMode, &testBtn](sdl2cpp::ui::EventInfo info,
                                             sdl2cpp::ui::MouseButton button,
                                             SDL_Point pos) {
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

  testBtn->text.setFont(guiManager.getFontManager().getFont("arialbd"));
  testBtn->text.setFontSize(60);

  FPSCounter fpsCounter;

  int cnt = 0;

  ChunkManager chunks{cameraController, config};

  mainLoop->setIdleCallback([&]() {
    ge::gl::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    chunks.generateChunks();
    chunks.draw(
        drawMode,
        {config.get<bool>("debug", "drawChunkBorder", "enabled").value(),
         config.get<bool>("debug", "drawNormals").value(),
         config.get<uint>("debug", "drawChunkBorder", "step").value()});

    auto projection =
        glm::perspective(glm::radians(60.f), 1920.f / 1080, 0.1f, 1000.0f);

    auto ortho = glm::ortho<float>(0, 1000, 0, 562.5, -1, 1);
    auto view = cameraController->getViewMatrix();
    guiManager.render(ortho /*, view*/);

    window->swap();

    fpsCounter.frame();

    if (cnt % 360 == 0)
      print(fpsCounter);

    ++cnt;
  });

  (*mainLoop)();

  return 0;
}
