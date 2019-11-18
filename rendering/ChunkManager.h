//
// Created by petr on 10/30/19.
//

#ifndef TERRAINGENERATION_CHUNKMANAGER_H
#define TERRAINGENERATION_CHUNKMANAGER_H

#include "../gui/CameraController.h"
#include "Chunk.h"
#include <Geometry.h>
#include <TransformFeedback.h>
#include <config/JsonConfig.h>
#include <fplus/fplus.hpp>
#include <geGL/StaticCalls.h>
#include <geGL/geGL.h>
#include <geGL_utils.h>
#include <loc_assert.h>
#include <print.h>

#include "Light.h"
#include "types.h"

namespace {
using Shader = std::shared_ptr<ge::gl::Shader>;
using Program = GLuint;
using Buffer = std::shared_ptr<ge::gl::Buffer>;
using VertexArray = std::shared_ptr<ge::gl::VertexArray>;

Logger logger{std::cout};
} // namespace

struct DrawOptions {
  const bool drawChunkArea;
  const bool drawNormals;
  const uint chunkAreaStep;
};

enum class ChunkIn {
  NotLoaded, Setup, Empty, Filled
};

class Surroundings {
public:
  Surroundings(float loadDistance, glm::uvec3 size, uint chunkPoolSize, float step)
      : loadDistance(loadDistance), size(size), step(step) {
    map.resize(size.x * size.y * size.z);
    chunkPool.resize(chunkPoolSize, {{0, 0, 0}, step, 32});
    for (uint i = 0; i < map.size(); ++i) {
      uint x = i % size.x;
      uint y = i / size.x % size.y;
      uint z = i / (size.x * size.y) % size.z;
      map[i] = {ChunkIn::NotLoaded, nullptr, glm::vec3{x, y, z} * step * 32.f};
    }
    for (auto &chunk : chunkPool) {
      available.emplace_back(&chunk);
    }
  }

  std::list<Chunk*> getForCompute(glm::vec3 position) {
    uint availableCount = available.size();
    uint emptyCount = 0;
    uint setupCount = 0;
    uint filledCount = 0;
    uint notLoadedCount = 0;
    constexpr uint availableThreshold = 30;
    for (auto &[state, ptr, pos] : map) {
      if (state == ChunkIn::NotLoaded && availableCount != 0) {;
        if (glm::distance(pos + 16.f * glm::vec3{step}, position) <= loadDistance) {
          auto chunk = available.front();
          available.remove(chunk);
          --availableCount;
          chunk->startPosition = pos;
          chunk->recalc();
          used.emplace_back(chunk);
          state = ChunkIn::Setup;
          ptr = chunk;
          ++setupCount;
        } else {
          ++notLoadedCount;
        }
      } else if (state == ChunkIn::Filled) {
        if (ptr->boundingSphere.distance(position) > loadDistance && availableCount < availableThreshold) {
          ptr->setComputed(false);
          const auto tmp = used.size();
          used.remove(ptr);
          available.emplace_back(ptr);
          ++availableCount;
          state = ChunkIn::NotLoaded;
          ptr = nullptr;
        }
        ++filledCount;
      } else if (state == ChunkIn::Empty) {
        if (ptr != nullptr) {
          available.emplace_back(ptr);
          used.remove(ptr);
          ++availableCount;
          ptr = nullptr;
        }
        ++emptyCount;
      }
    }
    const uint usedCount = used.size();
    if (usedCount != logger.recall<uint>("Used_chuk_count")) {
      print("Used: ", usedCount);
      logger.remember<uint>("Used_chuk_count", usedCount);
      print("Available count: ", available.size());
      print("emptyCount: ", emptyCount);
      print("setupCount: ", setupCount);
      print("filledCount: ", filledCount);
      print("notLoadedCount: ", notLoadedCount);
    }
    return used;
  }

  void setEmpty(Chunk *chunk) {
    auto tmp = [chunk] (auto &val){return std::get<1>(val) == chunk; };
    if (auto iter = std::find_if(map.begin(), map.end(), tmp); iter != map.end()) {
      std::get<0>(*iter) = ChunkIn::Empty;
    }
  }

  void setFilled(Chunk *chunk) {
    auto tmp = [chunk] (auto &val){return std::get<1>(val) == chunk; };
    if (auto iter = std::find_if(map.begin(), map.end(), tmp); iter != map.end()) {
      std::get<0>(*iter) = ChunkIn::Filled;
    }
  }

  const float loadDistance;
  const glm::uvec3 size;
  const float step;
private:
  std::vector<std::tuple<ChunkIn, Chunk*, glm::vec3>> map;
  std::vector<Chunk> chunkPool;
  std::list<Chunk*> available;
  std::list<Chunk*> used;
};

constexpr float step = 2;
constexpr uint size = 32;
class ChunkManager {
  Surroundings surr;
  std::list<Chunk*> chunks;
  CameraController &cameraController;

  BlinnPhongLight light{
      {1, 1, 1}, {50, 50, 50}, 10000, {1, 1, 1}, {1, 0, 1}, {1, 1, 1},
  };
  BlinnPhongMaterial material{10, {1, 1, 1}};

public:
  explicit ChunkManager(CameraController &cameraController, JsonConfig<true> config);

  void generateChunks();

  void draw(DrawMode mode, DrawOptions = {false, false});

private:
  JsonConfig<true> config;
  void drawChunk(const std::vector<Chunk *> &chunks, glm::mat4 projection,
                 glm::mat4 modelView, glm::vec3 lightPos);
  void drawNormals(const std::vector<Chunk *> &chunks, glm::mat4 MVPmatrix);
  void drawChunkCubes(const std::vector<Chunk *> &chunks, glm::mat4 MVPmatrix,
                      uint step);

  void calculateDensity(const std::vector<Chunk *> &chunks);

  void streamIdxVert(const std::vector<Chunk *> &chunks,
                     ge::gl::AsynchronousQuery &query);

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
