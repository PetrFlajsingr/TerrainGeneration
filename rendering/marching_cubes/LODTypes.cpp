//
// Created by petr on 12/22/19.
//

#include "LODTypes.h"

LODData::LODData(unsigned int levelCount, float viewDistance, float chunkStep) : levelCount(levelCount) {
  using namespace MakeRange;
  distances.resize(levelCount + 1);
  steps.resize(levelCount + 1);
  distances[0] = viewDistance;
  steps[0] = chunkStep;
  for (auto i : range<float>(1, levelCount + 1)) {
    distances[i] = distances[i - 1] / 12;
    steps[i] = chunkStep / std::pow(2, i);
  }
}

unsigned int LODData::ChunkCountInRow(unsigned int level) {
  if (level == 0) {
    return 1;
  }
  return ChunkCountInRow(level - 1) * 2;
}

LODTreeData::LODTreeData() : chunk(nullptr) {}
LODDir LODTreeData::getDir(glm::vec3 cameraPosition, const LODData &data) {
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

LOD::LOD(const LOD &other) { tree = other.tree; }

void LOD::initTree(std::size_t depth) {
  tree = Tree<LODTreeData, 8>::BuildTree(depth, LODTreeData{});
  setLevelInTree(tree.getRoot(), 0, depth);
}

void LOD::setLevelInTree(Leaf<LODTreeData, 8> &node, std::size_t level, std::size_t maxLevel) {
  node->level = level;
  if (node.getType() == NodeType::Leaf) {
    return;
  }
  for (auto &child : node.asNode().getChildren()) {
    setLevelInTree(*child, level + 1, maxLevel);
  }
}
