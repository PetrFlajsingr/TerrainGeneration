#include <cstdlib>
#include <iostream>

#include <SDL2CPP/MainLoop.h>
#include <SDL2CPP/Window.h>
#include <geGL/StaticCalls.h>
#include <geGL/geGL.h>

#include <FPSCounter.h>
#include <exceptions.h>
#include <experimental/array>
#include <fstream>
#include <glm/glm.hpp>
#include <print.h>

#include "gui/CameraController.h"
#include "marching_cubes/DensityGenerators.h"
#include "marching_cubes/FastChunkGen.h"
#include "marching_cubes/lookuptables.h"
#include "third_party/Camera.h"

using namespace std::string_literals;

int main(int, char *[]) {
  // create window
  const float aspectRatio = 1920.f / 1080;
  auto mainLoop = std::make_shared<sdl2cpp::MainLoop>();
  auto window = std::make_shared<sdl2cpp::Window>(1920, 1080);
  window->createContext("rendering", 430);
  mainLoop->addWindow("mainWindow", window);

  // init OpenGL
  ge::gl::init(SDL_GL_GetProcAddress);
  ge::gl::setHighDebugMessage();

  ge::gl::glClearColor(0, 0, 0, 1);


  setShaderLocation("/home/petr/CLionProjects/TerrainGeneration/marching_cubes/shaders");

  Compute compute;

  window->setEventCallback(SDL_KEYDOWN, compute.cameraController.getKeyboardCallback());
  window->setEventCallback(SDL_MOUSEMOTION,
                           compute.cameraController.getMouseMoveCallback());
  window->setEventCallback(SDL_MOUSEBUTTONDOWN,
                           compute.cameraController.getMouseDownCallback());
  window->setEventCallback(SDL_MOUSEBUTTONUP,
                           compute.cameraController.getMouseUpCallback());


  FPSCounter fpsCounter;
  // draw loop
  int cnt = 0;

  mainLoop->setIdleCallback([&]() {
    ge::gl::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    compute();
    window->swap();

    fpsCounter.step();

    if (cnt % 1000 == 0)
      std::cout << fpsCounter << '\n';

    ++cnt;
  });

  (*mainLoop)();

  return 0;
}
