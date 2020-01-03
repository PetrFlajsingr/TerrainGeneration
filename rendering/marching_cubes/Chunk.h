//
// Created by petr on 11/17/19.
//

#ifndef TERRAINGENERATION_CHUNK_H
#define TERRAINGENERATION_CHUNK_H

#include "graphics/BoundingBox.h"
#include "graphics/BoundingSphere.h"
#include "graphics/geGL_utils.h"
#include <geGL/Buffer.h>
#include <geGL/Generated/OpenGLTypes.h>
#include <geGL/VertexArray.h>
#include <glm/vec3.hpp>
#include <memory>

#include <geGL/GLSLNoise.h>
class Chunk {
  using SBuffer = std::shared_ptr<SparseBuffer>;
  using VertexArray = std::shared_ptr<ge::gl::VertexArray>;

public:
  /**
   * Prepares GPU buffers and VAO for rendering.
   * @param startPosition world space position for start of chunk -- lower left back corner
   * @param step size of one cube within the chunk
   * @param size amount of cubes in each row of a chunk
   */
  Chunk(glm::vec3 startPosition, float step, unsigned int size);

  Chunk(const Chunk &other);
  Chunk &operator=(const Chunk &other);
  Chunk(Chunk &&other) = default;

  [[nodiscard]] const SBuffer &getDensityBuffer() const;
  [[nodiscard]] const SBuffer &getVertexBuffer() const;
  [[nodiscard]] const SBuffer &getNormalBuffer() const;
  [[nodiscard]] const SBuffer &getIndexBuffer() const;

  const VertexArray &getVA();

  [[nodiscard]] bool isComputed() const;

  void setComputed(bool val);
  void recalc();

  [[nodiscard]] const glm::vec3 &getStartPosition() const;
  [[nodiscard]] float getStep() const;
  [[nodiscard]] uint getSize() const;
  [[nodiscard]] unsigned int getVertexCount() const;
  [[nodiscard]] unsigned int getIndexCount() const;

  [[nodiscard]] const geo::BoundingBox<3> &getBoundingBox() const;
  [[nodiscard]] const geo::BoundingSphere<3> &getBoundingSphere() const;

  void setStartPosition(const glm::vec3 &startPosition);
  void setStep(float step);
  void setVertexCount(unsigned int vertexCount);
  void setIndexCount(unsigned int indexCount);

private:
  glm::vec3 startPosition;
  float step;
  uint size;
  unsigned int vertexCount = 0;
  unsigned int indexCount = 0;

  geo::BoundingBox<3> boundingBox;
  geo::BoundingSphere<3> boundingSphere;

  SBuffer densityBuffer;
  SBuffer vertexBuffer;
  SBuffer normalBuffer;
  SBuffer indexBuffer;

  VertexArray drawVertexArray;

  geo::BoundingBox<3> calcAABB();
  bool computed = false;

  geo::BoundingSphere<3> calcBS();
};
#endif // TERRAINGENERATION_CHUNK_H
