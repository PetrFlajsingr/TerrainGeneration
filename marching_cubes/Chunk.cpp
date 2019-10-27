//
// Created by petr on 10/26/19.
//

#include "Chunk.h"
#include <glm/vec4.hpp>
mc::Chunk::Chunk(Chunk::Size size, float width, glm::vec3 position,
                 glm::vec4 color)
    : componentCount([this] { return std::pow(this->size, 3); }),
      position(position), size(size), id(idCounter), color(color),
      width(width) {
  ge::gl::glGenTransformFeedbacks(1, &feedbackName);
  initBuffers();
  ++idCounter;
}

bool mc::Chunk::isComputed() const { return computed; }

bool mc::Chunk::shouldBeDrawn() const { return hasDataToDraw; }

void mc::Chunk::dispatchDensityComputation(GLuint program, Blocking blocking) {

  vertexBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
  densityBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 1);
  cubeIndexBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 2);

  ge::gl::glUniform3fv(ge::gl::glGetUniformLocation(program, "start"), 1,
                       &position[0]);
  ge::gl::glUniform1f(ge::gl::glGetUniformLocation(program, "step"),
                      width / size);
  ge::gl::glDispatchCompute(4, 4, 4);
  if (blocking == Blocking::Yes) {
    ge::gl::glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
  }
  vertexBuffer->unbindBase(GL_SHADER_STORAGE_BUFFER, 0);
  densityBuffer->unbindBase(GL_SHADER_STORAGE_BUFFER, 1);
  cubeIndexBuffer->unbindBase(GL_SHADER_STORAGE_BUFFER, 2);
}

void mc::Chunk::dispatchCubeIndicesComputation(GLuint program,
                                               Blocking blocking) {
  vertexBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
  densityBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 1);
  cubeIndexBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 2);
  ge::gl::glDispatchCompute(4, 4, 4);
  if (blocking == Blocking::Yes) {
    ge::gl::glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
  }
  vertexBuffer->unbindBase(GL_SHADER_STORAGE_BUFFER, 0);
  densityBuffer->unbindBase(GL_SHADER_STORAGE_BUFFER, 1);
  cubeIndexBuffer->unbindBase(GL_SHADER_STORAGE_BUFFER, 2);
}

void mc::Chunk::calculateVertices(GLuint program) {

  ge::gl::AsynchronousQuery geometryQuery{
      GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, GL_QUERY_RESULT,
      ge::gl::AsynchronousQuery::UINT32};

  densityBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
  // polyCountLUTBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 1);
  // edgeLUTBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 2);
  // edgeToVertexLUTBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 3);
  cubeIndexBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 4);
  vertexBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 5);

  geometryVertexArray->bind();

  ge::gl::glUniform1f(ge::gl::glGetUniformLocation(program, "offset"), 0.);
  ge::gl::glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedbackName);
  ge::gl::glBeginTransformFeedback(GL_POINTS);
  geometryQuery.begin();

  ge::gl::glDrawArrays(GL_POINTS, 0, componentCount);

  geometryQuery.end();
  ge::gl::glEndTransformFeedback();

  densityBuffer->unbindBase(GL_SHADER_STORAGE_BUFFER, 0);
  cubeIndexBuffer->unbindBase(GL_SHADER_STORAGE_BUFFER, 4);
  vertexBuffer->unbindBase(GL_SHADER_STORAGE_BUFFER, 5);

  const auto primitiveCount = geometryQuery.getui();
  std::cout << *this << "\t|: primitive count: " << primitiveCount << std::endl;
  computed = true;
  hasDataToDraw = primitiveCount != 0;
}

void mc::Chunk::render(RenderType renderType, GLuint program) {

  if (!isComputed()) {
    throw std::logic_error("Chunk::render(...): Attempting to draw a chunk "
                           "which has not been computed yet.");
  }
  GLint primitiveType;
  glm::vec4 color = this->color;
  switch (renderType) {

  case Mesh:
    primitiveType = GL_TRIANGLES;
    break;
  case MeshLines:
    primitiveType = GL_TRIANGLES;
    break;
  case Normals:
    primitiveType = GL_POINTS;
    color = {1.0, 0, 1.0, 1.0};
    break;
  }
  if (shouldBeDrawn()) {
    ge::gl::glUniform4fv(ge::gl::glGetUniformLocation(program, "color"), 1,
                         &color[0]);
    drawVertexArray->bind();
    ge::gl::glDrawTransformFeedback(primitiveType, feedbackName);
  }
}

void mc::Chunk::initBuffers() {
  const Size chunkComponentCount = componentCount;
  vertexBuffer = std::make_shared<ge::gl::Buffer>(
      chunkComponentCount * sizeof(float) * 4, nullptr, GL_DYNAMIC_DRAW);

  densityBuffer = std::make_shared<ge::gl::Buffer>(
      chunkComponentCount * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
  cubeIndexBuffer = std::make_shared<ge::gl::Buffer>(
      chunkComponentCount * sizeof(uint), nullptr, GL_DYNAMIC_DRAW);
  vertexFeedbackBuffer = std::make_shared<ge::gl::Buffer>(
      chunkComponentCount * maxVerticesPerCube * 4 * sizeof(float), nullptr,
      GL_DYNAMIC_DRAW);
  normalFeedbackBuffer = std::make_shared<ge::gl::Buffer>(
      chunkComponentCount * maxVerticesPerCube * 3 * sizeof(float), nullptr,
      GL_DYNAMIC_DRAW);

  geometryVertexArray = std::make_shared<ge::gl::VertexArray>();
  geometryVertexArray->addAttrib(vertexBuffer, 0, 4, GL_FLOAT,
                                 static_cast<GLsizei>(sizeof(float) * 4), 0);

  drawVertexArray = std::make_shared<ge::gl::VertexArray>();
  drawVertexArray->addAttrib(vertexFeedbackBuffer, 0, 4, GL_FLOAT,
                             static_cast<GLsizei>(sizeof(float) * 4), 0);
  drawVertexArray->addAttrib(normalFeedbackBuffer, 1, 3, GL_FLOAT,
                             static_cast<GLsizei>(sizeof(float) * 3), 0);

  ge::gl::glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedbackName);
  ge::gl::glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0,
                           vertexFeedbackBuffer->getId());
  ge::gl::glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1,
                           normalFeedbackBuffer->getId());
  ge::gl::glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}

std::ostream &mc::operator<<(std::ostream &stream, mc::Chunk &chunk) {
  stream << "Chunk #" << chunk.id << ", size: " << chunk.size << "x"
         << chunk.size << "x" << chunk.size;
  return stream;
}

uint mc::Chunk::getId() const { return id; }

void mc::Chunk::invalidate() {
  computed = false;
  hasDataToDraw = false;
}
