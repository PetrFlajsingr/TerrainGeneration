//
// Created by petr on 11/17/19.
//

#ifndef TERRAINGENERATION_CHUNK_H
#define TERRAINGENERATION_CHUNK_H

#include "geGL_utils.h"
#include "graphics/BoundingBox.h"
#include "graphics/BoundingSphere.h"
#include "graphics/Geometry.h"
#include <cmath>
#include <geGL/Buffer.h>
#include <geGL/Generated/OpenGLTypes.h>
#include <geGL/VertexArray.h>
#include <glm/vec3.hpp>
#include <memory>

namespace {
using Shader = std::shared_ptr<ge::gl::Shader>;
using Program = GLuint;
using Buffer = std::shared_ptr<ge::gl::Buffer>;
using VertexArray = std::shared_ptr<ge::gl::VertexArray>;
} // namespace


class Chunk {
public:
  enum Buffers {
    Density, Vertex, Normal, Index
  };
  Chunk(glm::vec3 startPosition, float step, uint size);

  Chunk(const Chunk &other) = delete;
  Chunk(Chunk &&other) = default;

  glm::vec3 startPosition;
  float step;
  uint size;

  geo::BoundingBox<3> boundingBox;
  geo::BoundingSphere<3> boundingSphere;

  Buffer getBuffer(Buffers bufferType);

  VertexArray getVA();

  [[nodiscard]] bool isComputed() const;

  void setComputed(bool val);

  void recalc() {
    boundingSphere = calcBS();
    boundingBox = calcAABB();
  }

  /* DEBUG */
  uint vertexCount = 0;
  /* \DEBUG */
  uint indexCount = 0;

private:
  Buffer densityBuffer;
  Buffer vertexBuffer;
  Buffer normalBuffer;
  Buffer indexBuffer;

  VertexArray drawVertexArray;

  geo::BoundingBox<3> calcAABB();
  bool computed = false;

  geo::BoundingSphere<3> calcBS();
};
#endif // TERRAINGENERATION_CHUNK_H
