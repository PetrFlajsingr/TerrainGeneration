//
// Created by petr on 12/22/19.
//

#ifndef TERRAINGENERATION_LODCHUNKCONTROLLER_H
#define TERRAINGENERATION_LODCHUNKCONTROLLER_H

#include "LODTypes.h"
#include "SurroundingsEnums.h"

class ChunkUsageManager;
class Tile;
class EmptyChunkChecker {
public:
  bool operator()(Leaf<LODTreeData, 8> &lodData) {
    if (lodData->isCurrent && lodData->chunk != nullptr && lodData->chunk->indexCount > 0) {
      empty = false;
    }
    return lodData->isDivided;
  }

  operator bool() { return empty; }

  [[nodiscard]] bool isEmpty() const { return empty; }

private:
  bool empty = true;
};

class LODChunkController {
public:
  struct Counters {
    unsigned int setupCount = 0;
    unsigned int notLoadedCount = 0;
  };
  enum class Mode { New, Recycle, FilledLODCheck };
  LODChunkController(ChunkUsageManager &chunkUsageManager, unsigned int levelCount, float viewDistance, float chunkStep);

  using TreeTraversalFnc = std::function<bool(Leaf<LODTreeData, 8> &lodData)>;

  TreeTraversalFnc getTraverseFnc(Mode mode, glm::vec3 position, Tile &tile);
  static TreeTraversalFnc getEmptyCheck();

  [[nodiscard]] LODData &getLODData() { return data; }
  [[nodiscard]] const LODData &getLODData() const { return data; }

  [[nodiscard]] const Counters &getCounters() const { return counters; }
  void resetCounters();

  static glm::vec3 offsetForSubChunk(unsigned int index, unsigned int maxValue);

private:
  ChunkUsageManager &chunkUsageManager;

  LODData data;

  Counters counters;

  TreeTraversalFnc fncForNew(glm::vec3 position, Tile &tile);
  TreeTraversalFnc fncRecycle();
  TreeTraversalFnc fncLODCheck(glm::vec3 position, Tile &tile);
};

#endif // TERRAINGENERATION_LODCHUNKCONTROLLER_H
