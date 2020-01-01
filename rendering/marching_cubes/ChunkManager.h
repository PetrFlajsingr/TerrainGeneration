//
// Created by petr on 10/30/19.
//

#ifndef TERRAINGENERATION_CHUNKMANAGER_H
#define TERRAINGENERATION_CHUNKMANAGER_H

#include "Chunk.h"
#include "graphics/BoundingBox.h"
#include "graphics/Geometry.h"
#include "ui/elements/CameraController.h"
#include "various/loc_assert.h"
#include <config/JsonConfig.h>
#include <geGL/StaticCalls.h>
#include <graphics/TransformFeedback.h>
#include <graphics/geGL_utils.h>
#include <list>
#include <memory>
#include <vector>

#include "Surroundings.h"
#include "rendering/Data.h"
#include "rendering/Light.h"
#include "types.h"

namespace {
using Shader = std::shared_ptr<ge::gl::Shader>;
using Program = GLuint;
using Buffer = std::shared_ptr<ge::gl::Buffer>;
using VertexArray = std::shared_ptr<ge::gl::VertexArray>;

} // namespace

struct DrawOptions {
  const bool drawChunkArea;
  const bool drawNormals;
  const uint chunkAreaStep;
};

constexpr float step = 2;
constexpr uint size = 32;
class ChunkManager {
public:
  std::shared_ptr<sdl2cpp::ui::CameraController> cameraController;

  BlinnPhongLight light{
      {1, 1, 1}, {500, 500, 500}, 50000, {1, 1, 1}, {1, 1, 1}, {1, 1, 1},
  };
  BlinnPhongMaterial material{10, {1, 1, 1}};

  RenderData renderData;

  bool render = true;
  std::shared_ptr<ge::gl::Program> smProgram;
  std::shared_ptr<ge::gl::Texture> texture;
  Surroundings surr;

  explicit ChunkManager(std::shared_ptr<sdl2cpp::ui::CameraController> cameraController, JsonConfig<true> config);

  void generateChunks();
  void draw(DrawMode mode, DrawOptions = {false, false, 0});
  void drawToShadowMap(const geo::BoundingBox<3> &aabb);

private:
  JsonConfig<true> config;
  void drawChunk(const std::vector<Chunk *> &chunks, glm::mat4 projection);
  void drawNormals(const std::vector<Chunk *> &chunks, glm::mat4 MVPmatrix);
  void drawChunkCubes(const std::vector<Chunk *> &chunks, glm::mat4 MVPmatrix, uint step);

  void calculateDensity(const std::vector<Chunk *> &chunks);

  void streamIdxVert(const std::vector<Chunk *> &chunks, ge::gl::AsynchronousQuery &query);

  Shader generateDensity;
  Shader streamCases;
  Shader streamEdgeMarkers;
  Shader generateVertices;
  Shader clearVertexIDs;
  Shader splatVertices;
  Shader genIndices;
  Shader passThrough2;
  Shader passThrough3;
  Shader passThrough6;
  Shader drawCube;
  Shader simpleFrag;
  Shader cubePass;

  Shader gsNormalShader;
  Shader vsNormalShader;
  Shader fsShader;
  Shader fsBlinPhongShader;
  Shader vsBlinPhongShader;

  Program generateDensityProgram;
  Program streamCasesProgram;
  Program streamEdgeMarkersProgram;
  Program generateVerticesProgram;
  Program clearVertexIDsProgram;
  Program splatVerticesProgram;
  Program generateIndicesProgram;
  Program drawCubeBoundariesProgram;
  Program drawNormalsProgram;
  Program bpDrawProgram;

  Buffer polyCountLUTBuffer;
  Buffer edgeLUTBuffer;
  Buffer edgeToVertexLUTBuffer;
  Buffer vertexIDsBuffer;
  Buffer chunkCoordBuffer;
  Buffer caseBuffer;
  Buffer edgeBuffer;

  VertexArray chunkCoordVertexArray;
  VertexArray caseMarkersVertexArray;
  VertexArray edgeMarkersVertexArray;

  TransformFeedback transformFeedback1;
  TransformFeedback transformFeedback2;
  TransformFeedback transformFeedback3;
  TransformFeedback transformFeedback4;

  void loadShaders();
  void createLUT();
  void createBuffers();
  void createPrograms();
  void linkPrograms();
};

#endif // TERRAINGENERATION_CHUNKMANAGER_H
