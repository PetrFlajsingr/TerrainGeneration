//
// Created by petr on 11/17/19.
//

#include "Chunk.h"
#include "graphics/Geometry.h"
#include <error_handling/exceptions.h>

Chunk::Chunk(glm::vec3 startPosition, float step, unsigned int size)
    : startPosition(startPosition), step(step), size(size), boundingBox(calcAABB()), boundingSphere(calcBS()),
      densityBuffer(createSparseBuffer<float>(std::pow(size, 3))),
      vertexBuffer(createSparseBuffer<glm::vec4>(std::pow(size, 3) * 5)),
      normalBuffer(createSparseBuffer<glm::vec3>(std::pow(size, 3) * 5)),
      indexBuffer(createSparseBuffer<glm::uvec3>(std::pow(size, 3) * 5)),
      drawVertexArray(std::make_shared<ge::gl::VertexArray>()) {
  drawVertexArray->addAttrib(vertexBuffer, 0, 4, GL_FLOAT, sizeof(float) * 4, 0, GL_FALSE);
  drawVertexArray->addAttrib(normalBuffer, 1, 3, GL_FLOAT, sizeof(float) * 3, 0, GL_FALSE);
  drawVertexArray->addElementBuffer(indexBuffer);
}

const Chunk::VertexArray &Chunk::getVA() { return drawVertexArray; }

geo::BoundingBox<3> Chunk::calcAABB() {
  geo::BoundingBox<3> result;
  result.p1 = startPosition;
  result.p2 = result.p1 + size * step;
  return result;
}

geo::BoundingSphere<3> Chunk::calcBS() {
  geo::BoundingSphere<3> result;
  glm::vec3 p1 = startPosition;
  glm::vec3 p2 = startPosition + size * step;
  result.center = geo::midPoint(p1, p2);
  result.radius = glm::distance(result.center, p1);
  return result;
}

bool Chunk::isComputed() const { return computed; }

void Chunk::setComputed(bool val) { computed = val; }

void Chunk::recalc() {
  boundingSphere = calcBS();
  boundingBox = calcAABB();
}
Chunk::Chunk(const Chunk &other) {
  startPosition = other.startPosition;
  step = other.step;
  size = other.size;
  boundingBox = other.boundingBox;
  boundingSphere = other.boundingSphere;

  densityBuffer = createSparseBuffer<float>(std::pow(size, 3));
  vertexBuffer = createSparseBuffer<glm::vec4>(std::pow(size, 3) * 5);
  normalBuffer = createSparseBuffer<glm::vec3>(std::pow(size, 3) * 5);
  indexBuffer = createSparseBuffer<glm::uvec3>(std::pow(size, 3) * 5);
  drawVertexArray = std::make_shared<ge::gl::VertexArray>();
  drawVertexArray->addAttrib(vertexBuffer, 0, 4, GL_FLOAT, sizeof(float) * 4, 0, GL_FALSE);
  drawVertexArray->addAttrib(normalBuffer, 1, 3, GL_FLOAT, sizeof(float) * 3, 0, GL_FALSE);
  drawVertexArray->addElementBuffer(indexBuffer);
}
Chunk &Chunk::operator=(const Chunk &other) {
  if (this == &other) {
    return *this;
  }
  startPosition = other.startPosition;
  step = other.step;
  size = other.size;
  boundingBox = other.boundingBox;
  boundingSphere = other.boundingSphere;

  densityBuffer = createSparseBuffer<float>(std::pow(size, 3));
  vertexBuffer = createSparseBuffer<glm::vec4>(std::pow(size, 3) * 5);
  normalBuffer = createSparseBuffer<glm::vec3>(std::pow(size, 3) * 5);
  indexBuffer = createSparseBuffer<glm::uvec3>(std::pow(size, 3) * 5);
  drawVertexArray = std::make_shared<ge::gl::VertexArray>();
  drawVertexArray->addAttrib(vertexBuffer, 0, 4, GL_FLOAT, sizeof(float) * 4, 0, GL_FALSE);
  drawVertexArray->addAttrib(normalBuffer, 1, 3, GL_FLOAT, sizeof(float) * 3, 0, GL_FALSE);
  drawVertexArray->addElementBuffer(indexBuffer);
  return *this;
}
const Chunk::SBuffer &Chunk::getDensityBuffer() const { return densityBuffer; }
const Chunk::SBuffer &Chunk::getVertexBuffer() const { return vertexBuffer; }
const Chunk::SBuffer &Chunk::getNormalBuffer() const { return normalBuffer; }
const Chunk::SBuffer &Chunk::getIndexBuffer() const { return indexBuffer; }
const glm::vec3 &Chunk::getStartPosition() const { return startPosition; }
float Chunk::getStep() const { return step; }
uint Chunk::getSize() const { return size; }
unsigned int Chunk::getVertexCount() const { return vertexCount; }
unsigned int Chunk::getIndexCount() const { return indexCount; }
void Chunk::setStartPosition(const glm::vec3 &startPosition) { Chunk::startPosition = startPosition; }
void Chunk::setStep(float step) { Chunk::step = step; }
const geo::BoundingBox<3> &Chunk::getBoundingBox() const { return boundingBox; }
const geo::BoundingSphere<3> &Chunk::getBoundingSphere() const { return boundingSphere; }
void Chunk::setVertexCount(unsigned int vertexCount) { Chunk::vertexCount = vertexCount; }
void Chunk::setIndexCount(unsigned int indexCount) { Chunk::indexCount = indexCount; }
