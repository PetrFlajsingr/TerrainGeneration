//
// Created by petr on 11/22/19.
//

#include "Surroundings.h"
#include "io/print.h"
#include "utils/types/Zip.h"
#include <fplus/fplus.hpp>

Surroundings::Surroundings(float loadDistance, glm::uvec3 size,
                           unsigned int chunkPoolSize, float step)
    : loadDistance(loadDistance), size(size), step(step) {
  for (auto &map : maps) {
    map.tiles.resize(size.x * size.y * size.z);
  }
  chunkPool = fplus::generate<std::vector<Chunk>>(
      [step]() {
        return Chunk{{-30, -30, -30}, step, 32};
      },
      chunkPoolSize);

  const auto halfDist = 16.f * glm::vec3{step} * glm::vec3{size};

  using namespace MakeRange;
  const auto areaSize = step * 30.f;
  for (auto [index, coords] :
       zip(range(27), range<float, 3>({-1, -1, -1}, {2, 2, 2}, {1, 1, 1}))) {
    const auto [x, y, z] = coords;
    const auto startPos = glm::vec3{x, y, z} * glm::vec3{size} * areaSize;
    maps[index].init(startPos,
                     startPos + halfDist, size, step);
    print(startPos);
  }
  for (auto &chunk : chunkPool) {
    available.emplace_back(&chunk);
  }
}
std::list<Chunk *> Surroundings::getForCompute(glm::vec3 position) {
  //  TODO: move to GPU for speedup
  uint availableCount = available.size();
  uint emptyCount = 0;
  uint setupCount = 0;
  uint filledCount = 0;
  uint notLoadedCount = 0;
  constexpr uint availableThreshold = 30;
  unsigned int mapCnt = 0;
  for (auto &map : maps) {
    if (!map.isInRange(position, loadDistance * 10)) {
      continue;
    }
    ++mapCnt;

    for (auto &tile : map.tiles) {
      if (tile.state == ChunkIn::NotLoaded) {
        if (availableCount != 0 && setupCount < computeBatchSize &&
            glm::distance(tile.center, position) <= loadDistance) {
          auto chunk = available.front();
          available.remove(chunk);
          --availableCount;
          chunk->setComputed(false);
          chunk->startPosition = tile.pos;
          chunk->recalc();
          used.emplace_back(chunk);

          usedChunks[chunk] = &tile;

          tile.state = ChunkIn::Setup;
          tile.ptr = chunk;
          ++setupCount;
        } else {
          ++notLoadedCount;
        }
      } else if (tile.state == ChunkIn::Filled) {
        if ((aggressiveChunkUnloading || availableCount < availableThreshold) && tile.ptr->boundingSphere.distance(position) > loadDistance) {
          used.remove(tile.ptr);

          usedChunks[tile.ptr] = nullptr;

          available.emplace_back(tile.ptr);
          ++availableCount;
          ++notLoadedCount;
          tile.state = ChunkIn::NotLoaded;
          tile.ptr = nullptr;
        } else {
          ++filledCount;
        }
      } else if (tile.state == ChunkIn::Empty) {
        if (tile.ptr != nullptr) {
          available.emplace_back(tile.ptr);
          used.remove(tile.ptr);

          usedChunks[tile.ptr] = nullptr;

          ++availableCount;
          tile.ptr = nullptr;
        }
        ++emptyCount;
      } else if (tile.state == ChunkIn::Setup) {
        ++setupCount;
      }
    }
  }
  const uint usedCount = used.size();
  static int a = 0;
  ++a;
  info = WString::Format(L"Chunks: available {}, used {}, empty {}, setup {}, "
                         L"filled {}, notLoaded {}, maps {}",
                         available.size(), usedCount, emptyCount, setupCount,
                         filledCount, notLoadedCount, mapCnt);
  return used;
}
void Surroundings::setEmpty(Chunk *chunk) {
  if (auto iter = usedChunks.find(chunk); iter != usedChunks.end()) {
    iter->second->state = ChunkIn::Empty;
  }
}
void Surroundings::setFilled(Chunk *chunk) {
  if (auto iter = usedChunks.find(chunk); iter != usedChunks.end()) {
    iter->second->state = ChunkIn::Filled;
  }
}
