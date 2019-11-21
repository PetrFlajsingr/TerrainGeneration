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
      glm::vec3{0, 0, 0}, glm::vec3{1920, 1080, 0});

  auto testAction = guiManager.createGUIObject<KeyAction>(SDLK_r, [] {print("test mman");});

  auto testBtn = guiManager.createGUIObject<sdl2cpp::ui::Button>(
      glm::vec3{0, 0, 1}, glm::vec3{300, 300, 0});
  testBtn->text = "TEST";

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

    chunks.generateChunks();
    chunks.draw(
        DrawMode::Polygon,
        {config.get<bool>("debug", "drawChunkBorder", "enabled").value(),
         config.get<bool>("debug", "drawNormals").value(),
         config.get<uint>("debug", "drawChunkBorder", "step").value()});

    guiManager.render();

    window->swap();

    fpsCounter.frame();

    if (cnt % 360 == 0)
      print(fpsCounter);

    ++cnt;
  });

  (*mainLoop)();

  return 0;
}
