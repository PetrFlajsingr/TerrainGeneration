//
// Created by petr on 12/22/19.
//

#include "ChunkUsageManager.h"
#include "Surroundings.h"

ChunkPool initPool(ChunkPool::size_type chunkCount) {
  ChunkPool result;
  result.resize(chunkCount, Chunk{glm::vec3{-30, -30, -30}, 0, 32});
  return result;
}
ChunkUsageInitData::ChunkUsageInitData(std::size_t chunkPoolSize, std::size_t chunksPerFrameLimit, float loadingDistance,
                                       std::size_t LODlevelCount, float chunkStep, Unloading unloading)
    : chunkPoolSize(chunkPoolSize), chunksPerFrameLimit(chunksPerFrameLimit), loadingDistance(loadingDistance),
      LODlevelCount(LODlevelCount), chunkStep(chunkStep), unloading(unloading) {}

ChunkUsageManager::ChunkUsageManager(ChunkUsageInitData initData)
    : chunkPool(initPool(initData.chunkPoolSize)), chunksPerFrameLimit(initData.chunksPerFrameLimit),
      unloading(initData.unloading), loadingDistance(initData.loadingDistance),
      lodController(*this, initData.LODlevelCount, loadingDistance, initData.chunkStep) {
  std::transform(chunkPool.begin(), chunkPool.end(), std::back_inserter(data.available), [](auto &chunk) { return &chunk; });
}

void ChunkUsageManager::newFrame(glm::vec3 cameraPosition) {
  ChunkUsageManager::cameraPosition = cameraPosition;
  lodController.resetCounters();
}

void ChunkUsageManager::manageTile(Tile &tile) {
  switch (tile.state) {
  case ChunkState::NotLoaded:
    manageUnloadedTile(tile);
    break;
  case ChunkState::Setup:
    break;
  case ChunkState::MarkedEmpty:
    manageMarkedEmptyTile(tile);
    break;
  case ChunkState::Filled:
    manageFilledTile(tile);
    break;
  case ChunkState::Empty:
    break;
  }
}

bool ChunkUsageManager::hasAvailable() const { return !data.available.empty(); }

Chunk *ChunkUsageManager::borrowChunk() {
  auto result = data.available.front();
  data.used.emplace_back(result);
  data.available.remove(result);
  assert(result != nullptr);
  return result;
}

Chunk *ChunkUsageManager::borrowChunk(Tile &tile) {
  auto result = borrowChunk();
  data.chunkToTileMap[result] = &tile;
  assert(result != nullptr);
  return result;
}

void ChunkUsageManager::returnChunk(Chunk *chunk) {
  assert(chunk != nullptr);
  data.available.emplace_back(chunk);
  data.used.remove(chunk);
}

void ChunkUsageManager::returnTileChunk(Chunk *chunk) {
  returnChunk(chunk);
  data.chunkToTileMap[chunk] = nullptr;
}

void ChunkUsageManager::manageUnloadedTile(Tile &tile) {
  auto &counters = lodController.getCounters();
  if (hasAvailable() && counters.setupCount < chunksPerFrameLimit &&
      glm::distance(tile.center, cameraPosition) <= loadingDistance) {
    tile.lod.tree.traverseDepthFirstIf(lodController.getTraverseFnc(LODChunkController::Mode::New, cameraPosition, tile));
  }
}

void ChunkUsageManager::manageFilledTile(Tile &tile) {
  if (glm::distance(tile.center, cameraPosition) > loadingDistance) {
    tile.lod.tree.traverseDepthFirstIf(lodController.getTraverseFnc(LODChunkController::Mode::Recycle, cameraPosition, tile));
    tile.state = ChunkState::NotLoaded;
  } else {
    tile.lod.tree.traverseDepthFirstIf(lodController.getTraverseFnc(LODChunkController::Mode::FilledLODCheck, cameraPosition, tile));
  }
}

void ChunkUsageManager::manageMarkedEmptyTile(Tile &tile) {
  auto emptyCheck = LODChunkController::getEmptyCheck();
  tile.lod.tree.traverseDepthFirstIf(emptyCheck);

  if (emptyCheck) {
    tile.state = ChunkState::Empty;
    tile.lod.tree.traverseDepthFirstIf(lodController.getTraverseFnc(LODChunkController::Mode::Recycle, cameraPosition, tile));
  } else {
    tile.state = ChunkState::Filled;
  }
}
