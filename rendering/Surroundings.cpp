//
// Created by petr on 11/22/19.
//

#include "Surroundings.h"
#include "io/print.h"
#include <fplus/fplus.hpp>

Surroundings::Surroundings(float loadDistance, glm::uvec3 size,
                           uint chunkPoolSize, float step)
    : loadDistance(loadDistance), size(size), step(step) {
  map.resize(size.x * size.y * size.z);
  chunkPool = fplus::generate<std::vector<Chunk>>(
      [step]() {
        return Chunk{{-30, -30, -30}, step, 32};
      },
      chunkPoolSize);

  const auto halfDist = 16.f * glm::vec3{step};
  for (uint i = 0; i < map.size(); ++i) {
    uint x = i % size.x;
    uint y = i / size.x % size.y;
    uint z = i / (size.x * size.y) % size.z;
    const auto start = glm::vec3{x, y, z} * step * 30.f;
    const auto center = start + halfDist;
    map[i] = {ChunkIn::NotLoaded, nullptr, start, center};
  }
  for (auto &chunk : chunkPool) {
    available.emplace_back(&chunk);
  }
}
std::list<Chunk *> Surroundings::getForCompute(glm::vec3 position) {
  // TODO: move to GPU for speedup
  uint availableCount = available.size();
  uint emptyCount = 0;
  uint setupCount = 0;
  uint filledCount = 0;
  uint notLoadedCount = 0;
  constexpr uint availableThreshold = 30;
  for (auto &tile : map) {
    if (tile.state == ChunkIn::NotLoaded && availableCount != 0) {
      ;
      if (glm::distance(tile.center, position) <= loadDistance) {
        auto chunk = available.front();
        available.remove(chunk);
        --availableCount;
        chunk->setComputed(false);
        chunk->startPosition = tile.pos;
        chunk->recalc();
        used.emplace_back(chunk);
        tile.state = ChunkIn::Setup;
        tile.ptr = chunk;
        ++setupCount;
      } else {
        ++notLoadedCount;
      }
    } else if (tile.state == ChunkIn::Filled) {
      if (tile.ptr->boundingSphere.distance(position) > loadDistance &&
          (aggressiveChunkUnloading || availableCount < availableThreshold)) {
        const auto tmp = used.size();
        used.remove(tile.ptr);
        available.emplace_back(tile.ptr);
        ++availableCount;
        tile.state = ChunkIn::NotLoaded;
        tile.ptr = nullptr;
      }
      ++filledCount;
    } else if (tile.state == ChunkIn::Empty) {
      if (tile.ptr != nullptr) {
        available.emplace_back(tile.ptr);
        used.remove(tile.ptr);
        ++availableCount;
        tile.ptr = nullptr;
      }
      ++emptyCount;
    } else if (tile.state == ChunkIn::Setup) {
      ++setupCount;
    }
  }
  const uint usedCount = used.size();
  static int a = 0;
  ++a;
  if (a % 1 == 0) {
    print("Used: ", usedCount);
    print("Available count: ", available.size());
    print("emptyCount: ", emptyCount);
    print("setupCount: ", setupCount);
    print("filledCount: ", filledCount);
    print("notLoadedCount: ", notLoadedCount);
  }
  return used;
}
void Surroundings::setEmpty(Chunk *chunk) {
  // FIXME: this is really slow
  auto tmp = [chunk](auto &val) { return val.ptr == chunk; };
  if (auto iter = std::find_if(map.begin(), map.end(), tmp);
      iter != map.end()) {
    iter->state = ChunkIn::Empty;
  }
}
void Surroundings::setFilled(Chunk *chunk) {
  // FIXME: this is really slow
  auto tmp = [chunk](auto &val) { return val.ptr == chunk; };
  if (auto iter = std::find_if(map.begin(), map.end(), tmp);
      iter != map.end()) {
    iter->state = ChunkIn::Filled;
  }
}
