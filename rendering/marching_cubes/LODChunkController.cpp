//
// Created by petr on 12/22/19.
//

#include "LODChunkController.h"
#include "ChunkUsageManager.h"
#include "Surroundings.h"
#include <error_handling/exceptions.h>
#include <various/loc_assert.h>

LODChunkController::LODChunkController(ChunkUsageManager &chunkUsageManager, unsigned int levelCount, float viewDistance,
                                       float chunkStep)
    : chunkUsageManager(chunkUsageManager), data(levelCount, viewDistance, chunkStep) {}

LODChunkController::TreeTraversalFnc LODChunkController::getTraverseFnc(LODChunkController::Mode mode, glm::vec3 position,
                                                                        Tile &tile) {
  switch (mode) {
  case Mode::New:
    return fncForNew(position, tile);
  case Mode::Recycle:
    return fncRecycle();
  case Mode::FilledLODCheck:
    return fncLODCheck(position, tile);
  }
  throw exc::InternalError("Invalid state");
}

void LODChunkController::resetCounters() {
  counters.setupCount = 0;
  counters.notLoadedCount = 0;
}

glm::vec3 LODChunkController::offsetForSubChunk(unsigned int index, unsigned int maxValue) {
  static const std::array<const glm::vec3, 8> offsetsForSubChunks{glm::vec3{0, 0, 0}, glm::vec3{0, 0, 1}, glm::vec3{0, 1, 0},
                                                                  glm::vec3{0, 1, 1}, glm::vec3{1, 0, 0}, glm::vec3{1, 0, 1},
                                                                  glm::vec3{1, 1, 0}, glm::vec3{1, 1, 1}};
  glm::vec3 result{0, 0, 0};
  if (maxValue > 2) {
    result = 2.f * offsetForSubChunk(index / 8, maxValue / 2);
  }
  index = index % 8;
  return result + offsetsForSubChunks[index];
}

LODChunkController::TreeTraversalFnc LODChunkController::fncForNew(glm::vec3 position, Tile &tile) {
  return [this, position, &tile](Leaf<LODTreeData, 8> &lodData) {
    const LODDir lodDir = lodData->getDir(position, data);
    switch (lodDir) {
    case LODDir::Lower:
      lodData->isDivided = true;
      lodData->isCurrent = false;
      return true;
    case LODDir::Current: {
      lodData->isCurrent = true;
      lodData->isDivided = false;
      if (!chunkUsageManager.hasAvailable()) {
        return false;
      }
      lodData->chunk = chunkUsageManager.borrowChunk(tile);
      assert(lodData->chunk != nullptr);
      lodData->chunk->setComputed(false);
      const auto chunkStep = data.steps[lodData->level];
      lodData->chunk->setStep(chunkStep);
      lodData->chunk->setStartPosition(
          tile.pos + chunkStep * offsetForSubChunk(lodData->index, LODData::ChunkCountInRow(lodData->level)) * 30.f);
      lodData->chunk->recalc();
      tile.state = ChunkState::Setup;
      ++counters.setupCount;
    }
      return false;
    case LODDir::Higher:
      const bool isParentDivided = lodData.isRoot() ? false : lodData.getParent()->isDivided;
      if (isParentDivided) {
        lodData->isCurrent = true;
        lodData->isDivided = false;
        if (!chunkUsageManager.hasAvailable()) {
          return false;
        }
        lodData->chunk = chunkUsageManager.borrowChunk(tile);
        assert(lodData->chunk != nullptr);
        lodData->chunk->setComputed(false);
        const auto chunkStep = data.steps[lodData->level];
        lodData->chunk->setStep(chunkStep);
        lodData->chunk->setStartPosition(
            tile.pos + chunkStep * offsetForSubChunk(lodData->index, LODData::ChunkCountInRow(lodData->level)) * 30.f);
        lodData->chunk->recalc();
        tile.state = ChunkState::Setup;
        ++counters.setupCount;
      } else {
        loc_assert(lodData.isRoot(), "This should be unreachable as non-root node.");
        lodData->isDivided = false;
        lodData->isCurrent = false;
      }
      return false;
    }
    throw exc::InternalError("Invalid state");
  };
}

LODChunkController::TreeTraversalFnc LODChunkController::fncRecycle() {
  return [this](Leaf<LODTreeData, 8> &lodData) {
    if (lodData->isCurrent) {
      if (lodData->chunk != nullptr) {
        chunkUsageManager.returnTileChunk(lodData->chunk);
      }
      ++counters.notLoadedCount;
      lodData->chunk = nullptr;
      lodData->isCurrent = false;
    }
    const bool wasDivided = lodData->isDivided;
    lodData->isDivided = false;
    return wasDivided;
  };
}
LODChunkController::TreeTraversalFnc LODChunkController::fncLODCheck(glm::vec3 position, Tile &tile) {
  return [this, position, &tile](Leaf<LODTreeData, 8> &lodData) {
    const auto dir = lodData->getDir(position, data);
    if (lodData->isCurrent) {
      if (dir == LODDir::Current) {
        if (lodData->chunk == nullptr && chunkUsageManager.hasAvailable()) {
          lodData->chunk = chunkUsageManager.borrowChunk(tile);
        }
        return false;
      }
      if (dir == LODDir::Lower) {
        lodData->isDivided = true;
        lodData->isCurrent = false;
        chunkUsageManager.returnTileChunk(lodData->chunk);
        return true;
      }
      if (dir == LODDir::Higher) {
        const bool isParentDivided = lodData.isRoot() ? false : lodData.getParent()->isDivided;
        const bool wasDivided = lodData->isDivided;
        if (!isParentDivided) {
          lodData->isDivided = false;
          lodData->isCurrent = false;
          chunkUsageManager.returnTileChunk(lodData->chunk);
        }
        return wasDivided;
      }
      return false;
    } else if (dir == LODDir::Current) {
      const bool wasDivided = lodData->isDivided;
      lodData->isCurrent = true;
      lodData->isDivided = false;
      if (!chunkUsageManager.hasAvailable()) {
        return wasDivided;
      }
      lodData->chunk = chunkUsageManager.borrowChunk(tile);
      lodData->chunk->setComputed(false);
      const auto chunkStep = data.steps[lodData->level];
      lodData->chunk->setStep(chunkStep);
      lodData->chunk->setStartPosition(
          tile.pos + chunkStep * offsetForSubChunk(lodData->index, LODData::ChunkCountInRow(lodData->level)) * 30.f);
      lodData->chunk->recalc();
      ++counters.setupCount;
      return wasDivided;
    } else if (dir == LODDir::Higher) {
      const bool isParentDivided = lodData.isRoot() ? false : lodData.getParent()->isDivided;
      if (isParentDivided) {
        const bool wasDivided = lodData->isDivided;
        lodData->isCurrent = true;
        lodData->isDivided = false;
        if (!chunkUsageManager.hasAvailable()) {
          return false;
        }
        lodData->chunk = chunkUsageManager.borrowChunk(tile);
        lodData->chunk->setComputed(false);
        const auto chunkStep = data.steps[lodData->level];
        lodData->chunk->setStep(chunkStep);
        lodData->chunk->setStartPosition(
            tile.pos + chunkStep * offsetForSubChunk(lodData->index, LODData::ChunkCountInRow(lodData->level)) * 30.f);
        lodData->chunk->recalc();
        tile.state = ChunkState::Setup;
        ++counters.setupCount;
        return wasDivided;
      } else if (!lodData.isRoot()) {
        const bool wasDivided = lodData->isDivided;
        lodData->isDivided = false;
        if (lodData->isCurrent) {
          lodData->isCurrent = false;
          chunkUsageManager.returnTileChunk(lodData->chunk);
        }
        return wasDivided;
      }
      return lodData->isDivided;
    }
    return lodData->isDivided;
  };
}

EmptyChunkChecker LODChunkController::getEmptyCheck() { return EmptyChunkChecker(); }
LODData &LODChunkController::getLODData() { return data; }
const LODData &LODChunkController::getLODData() const { return data; }
const LODChunkController::Counters &LODChunkController::getCounters() const { return counters; }

bool EmptyChunkChecker::operator()(Leaf<LODTreeData, 8> &lodData) {
  if (lodData->isCurrent && lodData->chunk != nullptr && lodData->chunk->getIndexCount() > 0) {
    empty = false;
    return false;
  }
  return lodData->isDivided;
}

EmptyChunkChecker::operator bool() { return empty; }
bool EmptyChunkChecker::isEmpty() const { return empty; }
