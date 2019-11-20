#include <SDL2/SDL_image.h>
#include <SDL2CPP/MainLoop.h>
#include <SDL2CPP/Window.h>
#include <geGL/StaticCalls.h>
#include <geGL/geGL.h>

#include "io/print.h"
#include "time/FPSCounter.h"
#include <types.h>

#include "rendering/ChunkManager.h"
#include "rendering/Data.h"
#include "time/FPSCounter.h"
#include "ui/elements/Button.h"
#include "ui/elements/CameraController.h"
#include "ui/elements/KeyAction.h"
#include "ui/managers/GUIManager.h"
#include "utils/config/JsonConfig.h"

using namespace std::string_literals;

// TODO: custom types for config
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

  sdl2cpp::ui::GUIManager guiManager{window};

  auto cameraController = guiManager.createGUIObject<CameraController>(
      SDL_Rect{0, 0, static_cast<int>(deviceData.screen.width),
               static_cast<int>(deviceData.screen.height)});

  auto testAction = guiManager.createGUIObject<KeyAction>(SDLK_r, [] {print("test mman");});

  auto testBtn =
      guiManager.createGUIObject<sdl2cpp::ui::Button>(0, 0, 300, 300, 1);

  testBtn->setMouseOut([](sdl2cpp::ui::EventInfo info) { print("MouseOut"); })
      .setMouseOver([](sdl2cpp::ui::EventInfo info) { print("MouseOver"); })
      .setMouseClicked([](sdl2cpp::ui::EventInfo info,
                          sdl2cpp::ui::MouseButton button, SDL_Point pos) {
        print("Mouse clicked");
        std::string btn;
        switch (button) {
        case sdl2cpp::ui::MouseButton::Left:
          btn = "Left";
          break;
        case sdl2cpp::ui::MouseButton::Right:
          btn = "Right";
          break;
        case sdl2cpp::ui::MouseButton::Middle:
          btn = "Middle";
          break;
        }
        print("Button: ", btn);
      });

  FPSCounter fpsCounter;

  int cnt = 0;

  ChunkManager chunks{cameraController, config};
  mainLoop->setIdleCallback([&]() {
    ge::gl::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    guiManager.render();

    chunks.generateChunks();
    chunks.draw(
        DrawMode::Polygon,
        {config.get<bool>("debug", "drawChunkBorder", "enabled").value(),
         config.get<bool>("debug", "drawNormals").value(),
         config.get<uint>("debug", "drawChunkBorder", "step").value()});

    window->swap();

    fpsCounter.frame();

    if (cnt % 360 == 0)
      print(fpsCounter);

    ++cnt;
  });

  (*mainLoop)();

  return 0;
}
