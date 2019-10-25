#include <cstdlib>
#include <iostream>

#include <SDL2CPP/MainLoop.h>
#include <SDL2CPP/Window.h>
#include <geGL/StaticCalls.h>
#include <geGL/geGL.h>

#include <glm/glm.hpp>

#include "gui/CameraController.h"
#include "marching_cubes/DensityGenerators.h"
#include "marching_cubes/lookuptables.h"
#include "third_party/Camera.h"

namespace {
std::string static const vertexShaderSrc = R".(
#version 430

layout(location=0)in vec3 position;
layout(location=1)in int drawId;
layout(location=2)uniform mat4 mvpUniform;

out vec4 vColor;
void main(void) {
  vColor = vec4(1.0, 0.0, 0.0, 1.0);
  gl_Position = mvpUniform * vec4(position, 1.0);
}).";

std::string static const fragmentShaderSrc = R".(
#version 430

in vec4 vColor;
out vec4 fColor;
void main(void) {
    fColor = vColor;
}).";
}

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

  // create shader program
  auto vs = std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER, vertexShaderSrc);
  auto fs =
      std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER, fragmentShaderSrc);
  auto program = std::make_shared<ge::gl::Program>(vs, fs);

  // buffer data
  std::vector<float> vertices = {0.5f,  0.5f,  0.0f, 0.5f,  -0.5f, 0.0f,
                                 -0.5f, 0.5f,  0.0f, 0.5f,  -0.5f, 0.0f,
                                 -0.5f, -0.5f, 0.0f, -0.5f, 0.5f,  0.0f};
  std::vector<uint32_t> indices = {0, 1, 3, 1, 2, 3};

  auto vboBuffer = std::make_shared<ge::gl::Buffer>(
      vertices.size() * sizeof(decltype(vertices)::value_type), vertices.data(),
      GL_STATIC_DRAW);

  auto drawIdsBuffer = std::make_shared<ge::gl::Buffer>(
      indices.size() * sizeof(decltype(indices)::value_type), indices.data(),
      GL_STATIC_DRAW);

  auto vao = std::make_shared<ge::gl::VertexArray>();
  vao->addAttrib(vboBuffer, 0, 3, GL_FLOAT,
                 static_cast<GLsizei>(sizeof(float) * 3), 0);
  vao->addElementBuffer(drawIdsBuffer);

  ge::gl::glClearColor(0, 0, 0, 1);

  program->use();
  auto mvpMatrixUniformId =
      ge::gl::glGetUniformLocation(program->getId(), "mvpUniform");

  /*CameraController cameraController(glm::vec3(0, 0, 0.5));

  window->setEventCallback(SDL_KEYDOWN, cameraController.getKeyboardCallback());
  window->setEventCallback(SDL_MOUSEMOTION,
                           cameraController.getMouseMoveCallback());
  window->setEventCallback(SDL_MOUSEBUTTONDOWN,
                           cameraController.getMouseDownCallback());
  window->setEventCallback(SDL_MOUSEBUTTONUP,
                           cameraController.getMouseUpCallback());*/



  /*DensityBufferGenerator generator({{-1, 1, 0.1},
                                    {-1, 1, 0.1},
                                    {-1, 1, 0.1}});*/

  Compute compute({{0, 1, 8},
                   {0, 1, 8},
                   {0, 1, 8}});

  window->setEventCallback(SDL_KEYDOWN, compute.cameraController.getKeyboardCallback());
  window->setEventCallback(SDL_MOUSEMOTION,
                           compute.cameraController.getMouseMoveCallback());
  window->setEventCallback(SDL_MOUSEBUTTONDOWN,
                           compute.cameraController.getMouseDownCallback());
  window->setEventCallback(SDL_MOUSEBUTTONUP,
                           compute.cameraController.getMouseUpCallback());
  // draw loop
  mainLoop->setIdleCallback([&]() {
    ge::gl::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    compute();
    //generator.generateValues();
    /*auto view = cameraController.getViewMatrix();
    auto model = glm::mat4();
    auto projection =
        glm::perspective(glm::radians(90.f), aspectRatio, 0.1f, 100.0f);
    auto MVPmatrix = projection * view * model;
    ge::gl::glUniformMatrix4fv(mvpMatrixUniformId, 1, GL_FALSE,
                               &MVPmatrix[0][0]);

    program->use();
    vao->bind();
    ge::gl::glDrawArrays(GL_TRIANGLES, 0, 3);
    // ge::gl::glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    ge::gl::glBindVertexArray(0);*/
    window->swap();
  });

  (*mainLoop)();

  return 0;
}
