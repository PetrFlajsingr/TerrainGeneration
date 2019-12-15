//
// Created by petr on 11/22/19.
//

#ifndef TERRAINGENERATION_SURROUNDINGS_H
#define TERRAINGENERATION_SURROUNDINGS_H

#include "rendering/marching_cubes/Chunk.h"
#include <String.h>
#include <glm/glm.hpp>
#include <list>
#include <observable/value.hpp>
#include <types/Range.h>
#include <unordered_map>
#include <vector>

enum class ChunkIn { NotLoaded, Setup, Empty, Filled };

struct Tile {
  ChunkIn state;
  Chunk *ptr;
  glm::vec3 pos;
  glm::vec3 center;
};

struct Map {
  std::vector<Tile> tiles;
  glm::vec3 startPosition;
  glm::vec3 center;
  geo::BoundingSphere<3> boundingSphere;
  Map() = default;

  bool isInRange(glm::vec3 cameraPosition, float range) {
    return boundingSphere.distance(cameraPosition) <= range;
  }

  std::vector<Chunk *> init(glm::vec3 start, glm::vec3 center,
                            glm::uvec3 tileSize, float step) {
    using namespace MakeRange;
    startPosition = start;
    Map::center = center;
    std::vector<Chunk *> result;
    boundingSphere =
        geo::BoundingSphere<3>{center, glm::distance(start, center)};
    const auto halfDist = 30.f * glm::vec3{step};
    for (auto i : range(tiles.size())) {
      uint x = i % tileSize.x;
      uint y = i / tileSize.x % tileSize.y;
      uint z = i / (tileSize.x * tileSize.y) % tileSize.z;
      const auto start = glm::vec3{x, y, z} * step * 30.f;
      const auto center = start + halfDist;
      if (tiles[i].ptr != nullptr) {
        result.emplace_back(tiles[i].ptr);
      }
      tiles[i] = {ChunkIn::NotLoaded, nullptr, start + startPosition, center + startPosition};
    }
    return result;
  }
};

class Surroundings {

  bool aggressiveChunkUnloading = true;
  unsigned int computeBatchSize = 100;

public:
  Surroundings(float loadDistance, glm::uvec3 size, unsigned int chunkPoolSize,
               float step);

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
};

#endif // TERRAINGENERATION_SURROUNDINGS_H
