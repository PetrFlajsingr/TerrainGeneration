//
// Created by petr on 10/26/19.
//

#ifndef TERRAINGENERATION_CHUNK_H
#define TERRAINGENERATION_CHUNK_H

#include <Lazy.h>
#include <geGL/StaticCalls.h>
#include <geGL/geGL.h>
#include <glm/glm.hpp>
#include <types.h>
#include <valarray>

namespace mc {
class Chunk {
  static inline uint idCounter = 0;

public:
  using Size = uint32_t;
  Chunk(Size size, glm::vec3 position, glm::vec4 color);

  Lazy<Size> componentCount;

  [[nodiscard]] bool isComputed() const;

  [[nodiscard]] bool shouldBeDrawn() const;

 void
  dispatchDensityComputation(GLuint program,
                             Blocking blocking = Blocking::Yes);

  void dispatchCubeIndicesComputation(GLuint program,
      Blocking blocking = Blocking::Yes);

  void calculateVertices(GLuint program);

  void render(GLuint program);

  void invalidate() {
    computed = false;
    hasDataToDraw = false;
  }

  friend std::ostream &operator<<(std::ostream &stream, Chunk &chunk);

  uint getId() const;

  GLuint feedbackName;
  std::shared_ptr<ge::gl::Buffer> vertexBuffer;         // vec3
private:
  uint id;
  static inline constexpr Size maxVerticesPerCube = 5;
  static inline constexpr Size verticesPerTriangle = 4;

  const Size size;
  const glm::vec3 position;
  const glm::vec4 color;

  bool computed = false;
  bool hasDataToDraw = false;

  std::shared_ptr<ge::gl::Buffer> densityBuffer;        // float
  std::shared_ptr<ge::gl::Buffer> cubeIndexBuffer;      // uint8
  std::shared_ptr<ge::gl::Buffer> vertexFeedbackBuffer; // vec3
  std::shared_ptr<ge::gl::Buffer> normalFeedbackBuffer; // vec3

  std::shared_ptr<ge::gl::VertexArray>  drawVertexArray; // vertex vec3, normal vec3
  std::shared_ptr<ge::gl::VertexArray>  geometryVertexArray; // vertex vec3, normal vec3



  void initBuffers();
};
}
#endif // TERRAINGENERATION_CHUNK_H
