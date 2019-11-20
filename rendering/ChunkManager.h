//
// Created by petr on 10/30/19.
//

#ifndef TERRAINGENERATION_CHUNKMANAGER_H
#define TERRAINGENERATION_CHUNKMANAGER_H

#include "Chunk.h"
#include "graphics/Geometry.h"
#include "io/print.h"
#include "ui/elements/CameraController.h"
#include "various/loc_assert.h"
#include <TransformFeedback.h>
#include <config/JsonConfig.h>
#include <fplus/fplus.hpp>
#include <geGL/StaticCalls.h>
#include <geGL/geGL.h>
#include <geGL_utils.h>

#include "Data.h"
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
  struct Tile {
    ChunkIn state;
    Chunk *ptr;
    glm::vec3 pos;
    glm::vec3 center;
  };
public:
  Surroundings(float loadDistance, glm::uvec3 size, uint chunkPoolSize, float step)
      : loadDistance(loadDistance), size(size), step(step) {
    map.resize(size.x * size.y * size.z);
    chunkPool = fplus::generate<std::vector<Chunk>>([step] () {return Chunk {{-30, -30, -30}, step, 32};}, chunkPoolSize);

    const auto halfDist = 16.f * glm::vec3{step};
    for (uint i = 0; i < map.size(); ++i) {
      uint x = i % size.x;
      uint y = i / size.x % size.y;
      uint z = i / (size.x * size.y) % size.z;
      const auto start = glm::vec3{x, y, z} * step * 30.f;
      const auto center = start + halfDist;
      map[i] = {ChunkIn::NotLoaded, nullptr, start, center};
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
    for (auto &tile : map) {
      if (tile.state == ChunkIn::NotLoaded && availableCount != 0) {;
        if (glm::distance(tile.center, position) <= loadDistance) {
          auto chunk = available.front();
          available.remove(chunk);
          --availableCount;
          chunk->setComputed(false);
          chunk->startPosition = tile.pos;
          chunk->recalc();
          used.emplace_back(chunk);
          tile.state = ChunkIn::Setup;
          tile.ptr = chunk;
          ++setupCount;
        } else {
          ++notLoadedCount;
        }
      } else if (tile.state == ChunkIn::Filled) {
        if (tile.ptr->boundingSphere.distance(position) > loadDistance && availableCount < availableThreshold) {
          const auto tmp = used.size();
          used.remove(tile.ptr);
          available.emplace_back(tile.ptr);
          ++availableCount;
          tile.state = ChunkIn::NotLoaded;
          tile.ptr = nullptr;
        }
        ++filledCount;
      } else if (tile.state == ChunkIn::Empty) {
        if (tile.ptr != nullptr) {
          available.emplace_back(tile.ptr);
          used.remove(tile.ptr);
          ++availableCount;
          tile.ptr = nullptr;
        }
        ++emptyCount;
      }else if (tile.state == ChunkIn::Setup) {
        ++setupCount;
      }
    }
    const uint usedCount = used.size();
    static int a = 0;
    ++a;
    if (a % 200 == 0) {
      print("Used: ", usedCount);
      logger.remember<uint>("Used_chunk_count", setupCount);
      print("Available count: ", available.size());
      print("emptyCount: ", emptyCount);
      print("setupCount: ", setupCount);
      print("filledCount: ", filledCount);
      print("notLoadedCount: ", notLoadedCount);
    }
    return used;
  }

  void setEmpty(Chunk *chunk) {
    auto tmp = [chunk] (auto &val){return val.ptr == chunk; };
    if (auto iter = std::find_if(map.begin(), map.end(), tmp); iter != map.end()) {
      iter->state = ChunkIn::Empty;
    }
  }

  void setFilled(Chunk *chunk) {
    auto tmp = [chunk] (auto &val){return val.ptr == chunk; };
    if (auto iter = std::find_if(map.begin(), map.end(), tmp); iter != map.end()) {
      iter->state = ChunkIn::Filled;
    }
  }

  const float loadDistance;
  const glm::uvec3 size;
  const float step;
private:
  std::vector<Tile> map;
  std::vector<Chunk> chunkPool;
  std::list<Chunk*> available;
  std::list<Chunk*> used;
};

constexpr float step = 2;
constexpr uint size = 32;
class ChunkManager {
  Surroundings surr;
  std::list<Chunk*> chunks;
  std::shared_ptr<CameraController> cameraController;

  BlinnPhongLight light{
      {1, 1, 1}, {50, 50, 50}, 10000, {1, 1, 1}, {1, 0, 1}, {1, 1, 1},
  };
  BlinnPhongMaterial material{10, {1, 1, 1}};

  RenderData renderData;
public:
  explicit ChunkManager(std::shared_ptr<CameraController> cameraController, JsonConfig<true> config);

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
