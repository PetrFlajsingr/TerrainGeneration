#include <cstdlib>
#include <iostream>

#include <SDL2CPP/MainLoop.h>
#include <SDL2CPP/Window.h>
#include <geGL/StaticCalls.h>
#include <geGL/geGL.h>

#include <FPSCounter.h>
#include <print.h>

#include "gui/CameraController.h"
#include "rendering/ChunkManager.h"
#include "third_party/Camera.h"

using namespace std::string_literals;

int main(int, char *[]) {
  // create window
  auto mainLoop = std::make_shared<sdl2cpp::MainLoop>();
  auto window = std::make_shared<sdl2cpp::Window>(1920, 1080);
  window->createContext("rendering", 430);
  mainLoop->addWindow("mainWindow", window);

  // init OpenGL
  ge::gl::init(SDL_GL_GetProcAddress);
  ge::gl::setHighDebugMessage();

  ge::gl::glClearColor(0, 0, 0, 1);


  setShaderLocation("/home/petr/CLionProjects/TerrainGeneration/rendering/shaders");

  CameraController cameraController;
  ChunkManager chunks{cameraController};
  window->setEventCallback(SDL_KEYDOWN, cameraController.getKeyboardCallback());
  window->setEventCallback(SDL_MOUSEMOTION,
                           cameraController.getMouseMoveCallback());
  window->setEventCallback(SDL_MOUSEBUTTONDOWN,
                           cameraController.getMouseDownCallback());
  window->setEventCallback(SDL_MOUSEBUTTONUP,
                           cameraController.getMouseUpCallback());


  FPSCounter fpsCounter;
  // draw loop
  int cnt = 0;

  mainLoop->setIdleCallback([&]() {
    ge::gl::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    chunks.generateChunks();
    chunks.draw(DrawMode::Polygon, {false, false, 16});

    window->swap();

    fpsCounter.step();

    if (cnt % 360 == 0)
      print(fpsCounter);

    ++cnt;
  });

  (*mainLoop)();

  return 0;
}
