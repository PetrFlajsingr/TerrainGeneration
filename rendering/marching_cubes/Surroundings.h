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
#include "utils/containers/Tree.h"

struct LODData {
  std::vector<float> distances;
  std::vector<float> steps;
  const unsigned int levelCount;

  LODData(unsigned int levelCount, float viewDistance, float chunkStep) : levelCount(levelCount) {
    using namespace MakeRange;
    distances.resize(levelCount + 1);
    steps.resize(levelCount + 1);
    distances[0] = viewDistance;
    steps[0] = chunkStep;
    for (auto i : range<float>(1, levelCount + 1)) {
      distances[i] = distances[i - 1] / 12;
      steps[i] = chunkStep / std::pow(2, i);
    }
    //std::reverse(distances.begin(), distances.end());
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

enum class LODDir {
  Lower, Current, Higher
};

struct LODTreeData {
  Chunk *chunk;
  geo::BoundingSphere<3> boundingSphere;
  bool isCurrent = false;
  bool isDivided = false;
  unsigned int index;
  unsigned int level;

  LODTreeData() : chunk(nullptr){}

  bool isCorrectLOD(glm::vec3 cameraPosition, LODData &data) {
    const auto distance = boundingSphere.distance(cameraPosition);
    const auto lowerBound = level == 0 ? 0 : data.distances[level - 1];
    return lowerBound < distance && distance <= data.distances[level];
  }

  LODDir getDir(glm::vec3 cameraPosition, const LODData &data) {
    const auto distance = boundingSphere.distance(cameraPosition);
    const auto lowerBound = level == data.levelCount - 1 ? 0 : data.distances[level + 1];
    if (distance < lowerBound) {
      return LODDir::Lower;
    }
    if (distance <= data.distances[level]) {
      return LODDir::Current;
    }
    return LODDir::Higher;
  }
};

struct LOD {
  Tree<LODTreeData, 8> tree;

  explicit LOD() = default;
  LOD(const LOD &other) {
    tree = other.tree;
  }

  void initTree(std::size_t depth) {
    tree = Tree<LODTreeData, 8>::BuildTree(depth, LODTreeData{});
    setLevelInTree(tree.getRoot(), 0, depth);
  }

  static void setLevelInTree(Leaf<LODTreeData, 8> &node, std::size_t level, std::size_t maxLevel) {
    node->level = level;
    if (node.getType() == NodeType::Leaf) {
      return;
    }
    for (auto &child : node.asNode().getChildren()) {
      setLevelInTree(*child, level + 1, maxLevel);
    }
  }
};

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
