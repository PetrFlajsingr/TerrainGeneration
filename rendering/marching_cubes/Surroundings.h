//
// Created by petr on 11/22/19.
//

#ifndef TERRAINGENERATION_SURROUNDINGS_H
#define TERRAINGENERATION_SURROUNDINGS_H

#include "SurroundingsEnums.h"
#include "rendering/marching_cubes/Chunk.h"
#include "utils/various/isin.h"
#include <String.h>
#include <glm/glm.hpp>
#include <list>
#include <observable/value.hpp>
#include <types/Range.h>
#include <unordered_map>
#include <vector>

struct Tile {
  ChunkState state;
  Chunk *ptr;
  glm::vec3 pos;
  glm::vec3 center;
};

struct Map {
  std::vector<Tile> tiles;
  glm::vec3 startPosition;
  glm::vec3 center;
  geo::BoundingSphere<3> boundingSphere;
  geo::BoundingBox<3> boundingBox;
  Map() = default;

  bool isInRange(glm::vec3 cameraPosition, float range);

  std::vector<Chunk *> init(glm::vec3 start, glm::vec3 center, glm::uvec3 tileSize, float step);
};

class Surroundings {
public:
  Surroundings(float loadDistance, glm::uvec3 size, unsigned int chunkPoolSize, float step);

  observable::value<WString> info;

  std::list<Chunk *> getForCompute(glm::vec3 position);

  void setEmpty(Chunk *chunk);

  void setFilled(Chunk *chunk);

  const float loadDistance;
  const glm::uvec3 size;
  const float step;

private:
  std::array<Map, 27> maps;
  std::vector<Chunk> chunkPool;
  std::list<Chunk *> available;
  std::list<Chunk *> used;

  std::unordered_map<Chunk *, Tile *> usedChunks;

  bool aggressiveChunkUnloading = true;
  unsigned int computeBatchSize = 100;

  // std::unordered_map<SurrPos, Map *> partsMap;
  std::array<Map *, 27> partsMap;

  void checkForMapMove(glm::vec3 cameraPosition);

  void moveSurroundings(SurrMoveDir direction);

  void rearrangeSurroundings(SurrPos newMid);

  std::vector<Chunk *> unused;
};

#endif // TERRAINGENERATION_SURROUNDINGS_H