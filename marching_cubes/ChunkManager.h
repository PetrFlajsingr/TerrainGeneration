//
// Created by petr on 10/30/19.
//

#ifndef TERRAINGENERATION_CHUNKMANAGER_H
#define TERRAINGENERATION_CHUNKMANAGER_H

#include "../gui/CameraController.h"
#include "Chunk.h"
#include <Geometry.h>
#include <TransformFeedback.h>
#include <fplus/fplus.hpp>
#include <geGL/StaticCalls.h>
#include <geGL/geGL.h>
#include <geGL_utils.h>
#include <print.h>

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
};

constexpr float step = 0.5;
constexpr uint size = 32;
class ChunkManager {
  std::vector<Chunk> chunks;
  CameraController &cameraController;

  void bah() {
    for (int x = 0; x < 2; ++x) {
      for (int y = 0; y < 2; ++y) {
        chunks.push_back({{x * (size - 2) * step, 0, y * (size - 2) * step}, step, size});
      }
    }
  }
public:
  explicit ChunkManager(CameraController &cameraController);

  void generateChunks();

  void draw(DrawMode mode, DrawOptions = {false, false});

private:
  void drawChunk(glm::mat4 projection, glm::mat4 modelView, glm::vec3 lightPos);
  void drawNormals(glm::mat4 MVPmatrix);
  void drawChunkCubes(glm::mat4 MVPmatrix);

  void calculateDensity(const std::vector<Chunk*> &chunks);

  void streamIdxVert(const std::vector<Chunk*> &chunks, ge::gl::AsynchronousQuery &query);

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
