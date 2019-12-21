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
#include <io/print.h>
#include <list>
#include <observable/value.hpp>
#include <types/Range.h>
#include <unordered_map>
#include <vector>

struct LODData {
  std::vector<float> distances;
  const unsigned int levelCount;

  LODData(unsigned int levelCount, float viewDistance) : levelCount(levelCount) {
    using namespace MakeRange;
    distances.resize(levelCount + 1);
    distances[0] = viewDistance;
    for (auto i : range<float>(1, levelCount + 1)) {
      distances[i] = distances[i - 1] / 12;
    }
    std::reverse(distances.begin(), distances.end());
    print(distances);
  }

  static unsigned int chunkCountForLevel(unsigned int level) {
    if (level == 0) {
      return 1;
    }
    return 8 * chunkCountForLevel(level - 1);
  }

  static unsigned int lenForLevel(unsigned int level) {
    if (level == 0) {
      return 1;
    }
    return lenForLevel(level - 1) * 2;
  }
};

struct LOD {
  Chunk *highestLevel = nullptr;
  std::vector<std::vector<Chunk *>> levels;
  const LODData *data;

  unsigned int currentLevel = 0;

  LOD() = default;
  explicit LOD(const LODData &data) : data(&data) {
    levels.resize(data.levelCount);
    unsigned int levelSize = 8;
    for (auto &level : levels) {
      level.resize(levelSize);
      levelSize = std::pow(levelSize, 2);
    }
  }


};

struct Tile {
  ChunkState state;
  LOD lod;
  glm::vec3 pos;
  glm::vec3 center;

  unsigned int getLODlevel(glm::vec3 cameraPosition) {
    const float radius = glm::distance(pos, center);
    using namespace MakeRange;
    const auto distanceToCenter = glm::distance(cameraPosition, center);
    for (auto i : range(lod.data->levelCount)) {
      if (distanceToCenter - radius < lod.data->distances[i]) {
        return lod.data->levelCount - i;
      }
    }
    return 0;
  }
};

struct Map {
  std::vector<Tile> tiles;
  glm::vec3 startPosition;
  glm::vec3 center;
  geo::BoundingSphere<3> boundingSphere;
  geo::BoundingBox<3> boundingBox;
  Map() = default;

  bool isInRange(glm::vec3 cameraPosition, float range);

  std::vector<Chunk *> init(glm::vec3 start, glm::vec3 center, glm::uvec3 tileSize, float step, const LODData &lodData);
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
  LODData lodData;
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
