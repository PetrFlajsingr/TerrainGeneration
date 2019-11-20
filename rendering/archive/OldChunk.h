//
// Created by petr on 10/26/19.
//

#ifndef TERRAINGENERATION_OLDCHUNK_H
#define TERRAINGENERATION_OLDCHUNK_H

#include "graphics/BoundingBox.h"
#include "graphics/BoundingSphere.h"
#include "graphics/Geometry.h"
#include "graphics/ViewFrustum.h"
#include "types/Lazy.h"
#include <geGL/StaticCalls.h>
#include <geGL/geGL.h>
#include <glm/glm.hpp>
#include <types.h>
#include <valarray>

namespace mc {
class OldChunk {
  static inline uint idCounter = 0;
public:
  enum RenderType {Mesh, MeshLines, Normals};

  using Size = uint32_t;
  OldChunk(Size size, float width, glm::vec3 position, glm::vec4 color);

  [[nodiscard]] bool isComputed() const;

  [[nodiscard]] bool shouldBeDrawn() const;

  [[nodiscard]] bool shouldBeDrawn(const geo::ViewFrustum &viewFrustum);

 void
  dispatchDensityComputation(GLuint program,
                             Blocking blocking = Blocking::Yes);

  void dispatchCubeIndicesComputation(GLuint program,
      Blocking blocking = Blocking::Yes);

  void calculateVertices(GLuint program);

  void render(RenderType renderType, GLuint program);

  void invalidate();

  friend std::ostream &operator<<(std::ostream &stream, OldChunk &chunk);

  [[nodiscard]] uint getId() const;


  GLuint feedbackName;
  std::shared_ptr<ge::gl::Buffer> vertexBuffer;         // vec3
private:
  uint id;
  static inline constexpr Size maxVerticesPerCube = 5;
  static inline constexpr Size verticesPerTriangle = 4;

  const Size size;
  const float width;
  const glm::vec3 position;
  const glm::vec4 color;

  bool computed = false;
  bool hasDataToDraw = false;

  std::shared_ptr<ge::gl::Buffer> densityBuffer;        // float
  std::shared_ptr<ge::gl::Buffer> cubeIndexBuffer;      // uint
  std::shared_ptr<ge::gl::Buffer> vertexFeedbackBuffer; // vec3
  std::shared_ptr<ge::gl::Buffer> normalFeedbackBuffer; // vec3

  std::shared_ptr<ge::gl::Buffer> bsLines; // vec3

  std::shared_ptr<ge::gl::VertexArray>  drawVertexArray; // vertex vec3, normal vec3
  std::shared_ptr<ge::gl::VertexArray>  geometryVertexArray; // vertex vec3, normal vec3

  std::shared_ptr<ge::gl::VertexArray>  bsVertexArray; // vertex vec3, normal vec3



  void initBuffers();



public:
  Lazy<Size> componentCount;

  geo::BoundingSphere<3> boundingSphere;
  geo::BoundingBox<3> boundingBox;

  geo::BoundingBox<3> calcBB() {
    geo::BoundingBox<3> result;
    result.p1 = position;
    result.p2 = position + width;
    return result;
  }

  geo::BoundingSphere<3> calcBS() {
    geo::BoundingSphere<3> result;
    glm::vec3 p1 = position;
    glm::vec3 p2 = position + width;
    result.center = geo::midPoint(p1, p2);
    result.radius = glm::distance(result.center, p1);
    return result;
  }
};
}
#endif // TERRAINGENERATION_OLDCHUNK_H
