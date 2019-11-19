#include <SDL2CPP/MainLoop.h>
#include <SDL2CPP/Window.h>
#include <geGL/StaticCalls.h>
#include <geGL/geGL.h>

#include <FPSCounter.h>
#include <print.h>
#include <types.h>

#include "gui/CameraController.h"
#include "gui/GUIManager.h"
#include "gui/KeyAction.h"
#include "rendering/ChunkManager.h"
#include "rendering/Data.h"
#include "third_party/Camera.h"
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

  // CameraController cameraController;

  sdl2cpp::ui::GUIManager guiManager{window};

  /*window->setEventCallback(SDL_KEYDOWN,
  cameraController.getKeyboardCallback());
  window->setEventCallback(SDL_MOUSEMOTION,
                           cameraController.getMouseMoveCallback());
  window->setEventCallback(SDL_MOUSEBUTTONDOWN,
                           cameraController.getMouseDownCallback());
  window->setEventCallback(SDL_MOUSEBUTTONUP,
                           cameraController.getMouseUpCallback());*/

  auto cameraController = guiManager.createGUIObject<CameraController>(
      SDL_Rect{0, 0, static_cast<int>(deviceData.screen.width),
               static_cast<int>(deviceData.screen.height)});

  auto testAction = guiManager.createGUIObject<KeyAction>(SDLK_r, [] {print("test mman");});

  FPSCounter fpsCounter;

  int cnt = 0;

  ChunkManager chunks{cameraController, config};
  mainLoop->setIdleCallback([&]() {
    ge::gl::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    chunks.generateChunks();
    chunks.draw(
        DrawMode::Line,
        {config.get<bool>("debug", "drawChunkBorder", "enabled").value(),
         config.get<bool>("debug", "drawNormals").value(),
         config.get<uint>("debug", "drawChunkBorder", "step").value()});

    window->swap();

    fpsCounter.step();

    if (cnt % 360 == 0)
      print(fpsCounter);

    ++cnt;
  });

  (*mainLoop)();

  return 0;
}
