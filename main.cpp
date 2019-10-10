#include <cstdlib>
#include <iostream>

#include <SDL2CPP/MainLoop.h>
#include <SDL2CPP/Window.h>
#include <geGL/StaticCalls.h>
#include <geGL/geGL.h>

#include <glm/glm.hpp>

std::string static const vertexShaderSrc = R".(
#version 430

struct ObjectData{
  vec2 position;
  vec2 size    ;
  vec4 color   ;
};

layout(binding=0,std430)buffer Objects{ObjectData objects[];};

layout(location=0)in vec2 position;
layout(location=1)in int drawId;

out vec4 vColor;

void main() {
  ObjectData objectData = objects[drawId];
  vColor = objectData.color;
  gl_Position = vec4(position*objectData.size + objectData.position,1,1);
}).";

std::string static const fragmentShaderSrc = R".(
#version 430

in vec4 vColor;
out vec4 fColor;

void main(){
  fColor=vColor;
}).";

using namespace ge::gl;

struct DrawArraysIndirectCommand {
  uint count;
  uint instanceCount;
  uint first;
  uint baseInstance;

  DrawArraysIndirectCommand(uint count, uint instanceCount, uint first,
                            uint baseInstance)
      : count(count), instanceCount(instanceCount), first(first),
        baseInstance(baseInstance) {}
};

int main(int, char *[]) {
  // create window
  auto mainLoop = std::make_shared<sdl2cpp::MainLoop>();
  auto window = std::make_shared<sdl2cpp::Window>();
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
  std::vector<float> vertices = {
      // triangle vertices
      -1.f,
      -1.f,
      +1.f,
      -1.f,
      -1.f,
      +1.f,

      // diamond vertices
      +0.f,
      -1.f,
      +1.f,
      +0.f,
      -1.f,
      +0.f,
      +0.f,
      +1.f,
  };
  std::vector<int32_t> drawIds = {0, 1, 2, 3, 4, 5, 6};

  struct ObjectData {
    glm::vec2 position;
    glm::vec2 size;
    glm::vec4 color;
  };
  std::vector<ObjectData> objectData({
                                         {glm::vec2(+0.5f, -0.5f), glm::vec2(0.03f), glm::vec4(1, 0, 0, 1)},
                                         {glm::vec2(+0.5f, -0.2f), glm::vec2(0.06f), glm::vec4(0, 1, 0, 1)},
                                         {glm::vec2(+0.5f, +0.2f), glm::vec2(0.10f), glm::vec4(0, 0, 1, 1)},
                                         {glm::vec2(+0.5f, +0.5f), glm::vec2(0.13f), glm::vec4(0, 1, 1, 1)},
                                         {glm::vec2(-0.5f, -0.5f), glm::vec2(0.03f), glm::vec4(1, 1, 1, 1)},
                                         {glm::vec2(-0.5f, +0.0f), glm::vec2(0.06f), glm::vec4(1, 1, 0, 1)},
                                         {glm::vec2(-0.5f, +0.5f), glm::vec2(0.10f), glm::vec4(1, 0, 1, 1)},
                                     });

  std::array<DrawArraysIndirectCommand, 2> commands{
      DrawArraysIndirectCommand{3, 4, 0, 0},
      DrawArraysIndirectCommand{4, 3, 3, 4}};

  // This buffer contains vertex positions.
  GLuint vbo;
  glCreateBuffers(1, &vbo);
  glNamedBufferData(vbo,
                    vertices.size() * sizeof(decltype(vertices)::value_type),
                    vertices.data(), GL_STATIC_DRAW);

  // This buffer contains drawIds.
  GLuint drawIdsBuffer;
  glCreateBuffers(1, &drawIdsBuffer);
  glNamedBufferData(drawIdsBuffer,
                    drawIds.size() * sizeof(decltype(drawIds)::value_type),
                    drawIds.data(), GL_STATIC_DRAW);

  // This buffer contains positions, sizes and colors of all instances.
  GLuint objectDataBuffer;
  glCreateBuffers(1, &objectDataBuffer);
  glNamedBufferData(objectDataBuffer,
                    objectData.size() *
                    sizeof(decltype(objectData)::value_type),
                    objectData.data(), GL_STATIC_DRAW);

  GLuint indirectBuffer;
  glCreateBuffers(1, &indirectBuffer);
  glNamedBufferData(indirectBuffer,
                    sizeof(DrawArraysIndirectCommand) * commands.size(),
                    commands.data(), GL_STATIC_DRAW);

  // vertex array object
  GLuint vao;
  glCreateVertexArrays(1, &vao);

  // attrib 0. is vertex positions
  glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayVertexBuffer(vao, 0, vbo, 0,
                            static_cast<GLsizei>(sizeof(float) * 2));
  glVertexArrayAttribBinding(vao, 0, 0);
  glEnableVertexArrayAttrib(vao, 0);
  glVertexArrayBindingDivisor(vao, 0, 0);

  // attrib 1. simulates gl_DrawID
  glVertexArrayAttribIFormat(vao, 1, 1, GL_INT, 0);
  glVertexArrayVertexBuffer(vao, 1, drawIdsBuffer, 0, sizeof(int32_t));
  glVertexArrayAttribBinding(vao, 1, 1);
  glEnableVertexArrayAttrib(vao, 1);
  glVertexArrayBindingDivisor(vao, 1, 1);

  glClearColor(0, 0, 0, 1);

  // draw loop
  mainLoop->setIdleCallback([&]() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    program->use();

    glBindVertexArray(vao);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, objectDataBuffer);

    ///\todo Homework 1.) Replace these drawing commands with single
    /// glMultiDrawArraysIndirect call.
    /// You have to create draw indirect buffer that contains correct values
    /// (draw commands). You will need these OpenGL functions:
    /// glMultiDrawArraysIndirect
    /// glCreateBuffers
    /// glNamedBufferData
    /// glBindBuffer

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
    glMultiDrawArraysIndirect(GL_TRIANGLE_STRIP, nullptr, commands.size(),
                              sizeof(DrawArraysIndirectCommand));

    glBindVertexArray(0);
    window->swap();
  });

  (*mainLoop)();

  return EXIT_SUCCESS;
}
