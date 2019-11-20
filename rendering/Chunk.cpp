//
// Created by petr on 11/17/19.
//

#include "Chunk.h"

Chunk::Chunk(glm::vec3 startPosition, float step, uint size)
    : densityBuffer(createBuffer<float>(std::pow(size, 3), GL_DYNAMIC_DRAW)),
      vertexBuffer(
          createBuffer<glm::vec4>(std::pow(size, 3) * 5, GL_DYNAMIC_DRAW)),
      normalBuffer(
          createBuffer<glm::vec3>(std::pow(size, 3) * 5, GL_DYNAMIC_DRAW)),
      indexBuffer(
          createBuffer<glm::uvec3>(std::pow(size, 3) * 5, GL_DYNAMIC_DRAW)),
      drawVertexArray(std::make_shared<ge::gl::VertexArray>()),
      startPosition(startPosition), step(step), size(size), boundingBox(calcAABB()),
      boundingSphere(calcBS()) {
  drawVertexArray->addAttrib(vertexBuffer, 0, 4, GL_FLOAT, sizeof(float) * 4, 0,
                             GL_FALSE);
  drawVertexArray->addAttrib(normalBuffer, 1, 3, GL_FLOAT, sizeof(float) * 3, 0,
                             GL_FALSE);
  drawVertexArray->addElementBuffer(indexBuffer);

}

Buffer Chunk::getBuffer(Chunk::Buffers bufferType) {
  switch (bufferType) {
  case Density:
    return densityBuffer;
  case Vertex:
    return vertexBuffer;
  case Normal:
    return normalBuffer;
  case Index:
    return indexBuffer;
  }
}

VertexArray Chunk::getVA() { return drawVertexArray; }

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
