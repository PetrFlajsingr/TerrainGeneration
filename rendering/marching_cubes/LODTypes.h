//
// Created by petr on 12/22/19.
//

#ifndef TERRAINGENERATION_LODTYPES_H
#define TERRAINGENERATION_LODTYPES_H

#include "Chunk.h"
#include "utils/containers/Tree.h"
#include <glm/glm.hpp>
#include <graphics/BoundingSphere.h>
#include <types/Range.h>
#include <vector>

struct LODData {
  std::vector<float> distances;
  std::vector<float> steps;
  const unsigned int levelCount;

  LODData(unsigned int levelCount, float viewDistance, float chunkStep);

  static unsigned int ChunkCountInRow(unsigned int level);
};

enum class LODDir { Lower, Current, Higher };

struct LODTreeData {
  Chunk *chunk;
  geo::BoundingSphere<3> boundingSphere;
  bool isCurrent = false;
  bool isDivided = false;
  unsigned int index;
  unsigned int level;

  LODTreeData();

  LODDir getDir(glm::vec3 cameraPosition, const LODData &data);
};

struct LOD {
  Tree<LODTreeData, 8> tree;

  explicit LOD() = default;
  LOD(const LOD &other);

  void initTree(std::size_t depth);

  static void setLevelInTree(Leaf<LODTreeData, 8> &node, std::size_t level, std::size_t maxLevel);
};

#endif // TERRAINGENERATION_LODTYPES_H
