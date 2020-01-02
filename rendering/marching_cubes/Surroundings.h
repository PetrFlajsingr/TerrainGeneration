//
// Created by petr on 11/22/19.
//

#ifndef TERRAINGENERATION_SURROUNDINGS_H
#define TERRAINGENERATION_SURROUNDINGS_H

#include "ChunkUsageManager.h"
#include "LODTypes.h"
#include "SurroundingsEnums.h"
#include "rendering/marching_cubes/Chunk.h"
#include "utils/containers/Tree.h"
#include "utils/parallel/ThreadPool.h"
#include "utils/various/isin.h"
#include <String.h>
#include <glm/glm.hpp>
#include <io/print.h>
#include <list>
#include <observable/value.hpp>
#include <types/Range.h>
#include <unordered_map>
#include <vector>

class ChunkUsageManager;

struct Tile {
  ChunkState state;
  LOD lod;
  glm::vec3 pos;
  glm::vec3 center;
  Tile() = default;
  Tile(ChunkState state, LOD lod, glm::vec3 pos, glm::vec3 center) : state(state), lod(lod), pos(pos), center(center) {}
};

struct Map {
  std::vector<Tile> tiles;
  glm::vec3 startPosition;
  glm::vec3 center;
  geo::BoundingSphere<3> boundingSphere;
  geo::BoundingBox<3> boundingBox;
  Map() = default;

  bool isInRange(glm::vec3 cameraPosition, float range);

  void init(glm::vec3 start, glm::vec3 center, glm::uvec3 tileSize, float step, const LODData &lodData);
  std::vector<Chunk *> restart(glm::vec3 start, glm::vec3 center, glm::uvec3 tileSize, float step, const LODData &lodData);
  std::vector<Chunk *> restartChunks();
};

class Surroundings {
public:
  Surroundings(float loadDistance, glm::uvec3 size, ChunkUsageManager &, float step);

  observable::value<WString> info;

  void checkDistances(glm::vec3 position);

  void setEmpty(Chunk *chunk);

  void setFilled(Chunk *chunk);
  void setNotLoaded(Chunk *chunk);

  void invalidate();
  const float loadDistance;
  const glm::uvec3 size;
  const float step;

private:
  glm::vec3 lastCameraPosition{std::numeric_limits<float>::infinity()};
  LODData lodData;
  std::array<Map, 27> maps;
  std::array<Map *, 27> partsMap;
  ChunkUsageManager &chunkUsageManager;

  void checkForMapMove(glm::vec3 cameraPosition);

  void moveSurroundings(SurrMoveDir direction);

  void rearrangeSurroundings(SurrPos newMid);

  std::vector<Chunk *> unused;
};

#endif // TERRAINGENERATION_SURROUNDINGS_H
