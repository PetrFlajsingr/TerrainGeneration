//
// Created by petr on 11/22/19.
//

#include "Surroundings.h"
#include "io/print.h"
#include "utils/types/Zip.h"
#include <error_handling/exceptions.h>

using namespace MakeRange;

Surroundings::Surroundings(float loadDistance, glm::uvec3 size, unsigned int chunkPoolSize, float step)
    : loadDistance(loadDistance), size(size), step(step), lodData(2, loadDistance, step) {
  for (auto &map : maps) {
    map.tiles.resize(size.x * size.y * size.z);
  }
  for ([[maybe_unused]] auto _ : range(chunkPoolSize)) {
    chunkPool.emplace_back(glm::vec3{-30, -30, -30}, step, 32);
  }

  const auto halfDist = 16.f * glm::vec3{step} * glm::vec3{size};

  using namespace MakeRange;
  const auto areaSize = step * 30.f;
  for (auto [index, coords] : zip(range(27), range<float, 3>({-1, -1, -1}, {2, 2, 2}, {1, 1, 1}))) {
    const auto [x, y, z] = coords;
    const auto startPos = glm::vec3{x, y, z} * glm::vec3{size} * areaSize;
    maps[index].init(startPos, startPos + halfDist, size, step, lodData);
    partsMap[index] = &maps[index];
  }
  for (auto &chunk : chunkPool) {
    available.emplace_back(&chunk);
  }
}

glm::vec3 forLOD(unsigned int index, unsigned int len) {
  glm::vec3 result{0, 0, 0};
  if (len > 2) {
    result = 2.f * forLOD(index / 8, len / 2);
  }
  index = index % 8;
  switch (index) {
  case 0:
    return result + glm::vec3{0, 0, 0};
  case 1:
    return result + glm::vec3{0, 0, 1};
  case 2:
    return result + glm::vec3{0, 1, 0};
  case 3:
    return result + glm::vec3{0, 1, 1};
  case 4:
    return result + glm::vec3{1, 0, 0};
  case 5:
    return result + glm::vec3{1, 0, 1};
  case 6:
    return result + glm::vec3{1, 1, 0};
  case 7:
    return result + glm::vec3{1, 1, 1};
  default:
    throw exc::InternalError("Invalid input value");
  }
}

std::list<Chunk *> Surroundings::getForCompute(glm::vec3 position) {
  checkForMapMove(position);
  for (auto ptr : unused) {
    assert(ptr != nullptr);
    available.emplace_back(ptr);
    usedChunks[ptr] = nullptr;
    used.remove(ptr);
  }
  unused.clear();

  struct {
    unsigned int availableCount = 0;
    unsigned int emptyCount = 0;
    unsigned int setupCount = 0;
    unsigned int filledCount = 0;
    unsigned int notLoadedCount = 0;
    unsigned int mapCnt = 0;
  } counters;

  constexpr uint availableThreshold = 30;

  //  FIXME: some issues with available counting
  for (auto &map : maps) {
    if (!map.isInRange(position, loadDistance)) {
      continue;
    }
    ++counters.mapCnt;

    for (auto &tile : map.tiles) {
      counters.availableCount = available.size();
      assert(counters.availableCount == available.size());
      if (tile.state == ChunkState::NotLoaded) {
        if (counters.availableCount != 0 && counters.setupCount < computeBatchSize &&
            glm::distance(tile.center, position) <= loadDistance) {
          tile.lod.tree.traverseDepthFirstIf([this, position, &tile, &counters](LODTreeData &data) {
            const LODDir lodDir = data.getDir(position, lodData);
            switch (lodDir) {
            case LODDir::Lower:
              data.isDivided = false;
              return false;
            case LODDir::Current: {
              data.isCurrent = true;
              data.isDivided = false;
              data.chunk = available.front();
              assert(data.chunk != nullptr);
              available.remove(data.chunk);
              data.chunk->setComputed(false);
              const auto chunkStep = lodData.steps[data.level];
              data.chunk->step = chunkStep;
              data.chunk->startPosition = tile.pos + chunkStep * forLOD(data.index, LODData::lenForLevel(data.level)) * 30.f;
              data.chunk->recalc();
              used.emplace_back(data.chunk);
              usedChunks[data.chunk] = &tile;
              tile.state = ChunkState::Setup;
              ++counters.setupCount;
            }
              return false;
            case LODDir::Higher:
              data.isDivided = true;
              data.isCurrent = false;
              return true;
            }
          });
        } else {
          ++counters.notLoadedCount;
        }
      } else if (tile.state == ChunkState::Filled) {
        if ((aggressiveChunkUnloading || counters.availableCount < availableThreshold) &&
            glm::distance(tile.center, position) > loadDistance) {
          tile.lod.tree.traverseDepthFirstIf([this, &counters](LODTreeData &data) {
            if (data.isCurrent) {
              const auto chunk = data.chunk;
              used.remove(chunk);
              usedChunks[chunk] = nullptr;
              assert(chunk != nullptr);
              available.emplace_back(chunk);
              ++counters.availableCount;
              ++counters.notLoadedCount;
              data.chunk = nullptr;
            }
            return data.isDivided;
          });
          tile.state = ChunkState::NotLoaded;
        } else {
          tile.lod.tree.traverseDepthFirstIf([this, &counters, &position, &tile](LODTreeData &data) {
            const auto dir = data.getDir(position, lodData);
            if (data.isCurrent) {
              if (dir == LODDir::Current) {
                return false;
              }
              data.isCurrent = false;
              if (dir == LODDir::Lower) {
                data.isDivided = true;
              }
              bool wasDivided = true;
              if (dir == LODDir::Higher) {
                wasDivided = data.isDivided;
                data.isDivided = false;
              }
              assert(data.chunk != nullptr);
              available.emplace_back(data.chunk);
              used.remove(data.chunk);
              usedChunks[data.chunk] = nullptr;
              ++counters.availableCount;
              return wasDivided;
            } else if (dir == LODDir::Current) {
              const bool wasDivided = data.isDivided;
              data.isCurrent = true;
              data.isDivided = false;
              data.chunk = available.front();
              assert(data.chunk != nullptr);
              available.remove(data.chunk);
              data.chunk->setComputed(false);
              const auto chunkStep = lodData.steps[data.level];
              data.chunk->step = chunkStep;
              data.chunk->startPosition = tile.pos + chunkStep * forLOD(data.index, LODData::lenForLevel(data.level)) * 30.f;
              data.chunk->recalc();
              used.emplace_back(data.chunk);
              usedChunks[data.chunk] = &tile;
              ++counters.setupCount;
              return wasDivided;
            } else if (dir == LODDir::Higher) {
              data.isDivided = true;
            }
            return data.isDivided;
          });
          ++counters.filledCount;
        }
      } else if (tile.state == ChunkState::MarkedEmpty) {
        bool isEmpty = true;
        tile.lod.tree.traverseDepthFirstIf([&isEmpty](LODTreeData &data) {
          if (data.isCurrent && data.chunk != nullptr && data.chunk->indexCount > 0) {
            isEmpty = false;
          }
          return data.isDivided;
        });

        if (isEmpty) {
          tile.state = ChunkState::Empty;
          tile.lod.tree.traverseDepthFirstIf([this, &counters](LODTreeData &data) {
            if (data.isCurrent) {
              const auto chunk = data.chunk;
              used.remove(chunk);
              usedChunks[chunk] = nullptr;
              assert(chunk != nullptr);
              available.emplace_back(chunk);
              ++counters.availableCount;
              ++counters.notLoadedCount;
              data.chunk = nullptr;
            }
            return data.isDivided;
          });
        } else {
          tile.state = ChunkState::Filled;
        }
        ++counters.emptyCount;
      } else if (tile.state == ChunkState::Setup) {
        ++counters.setupCount;
      } else if (tile.state == ChunkState::Empty) {
        ++counters.emptyCount;
      }
    }
  }
  const uint usedCount = used.size();
  static int a = 0;
  ++a;
  info = WString::Format(L"Chunks: available {}, used {}, empty {}, setup {}, "
                         L"filled {}, notLoaded {}, maps {}",
                         available.size(), usedCount, counters.emptyCount, counters.setupCount, counters.filledCount,
                         counters.notLoadedCount, counters.mapCnt);
  return used;
}
void Surroundings::setEmpty(Chunk *chunk) {
  if (auto iter = usedChunks.find(chunk); iter != usedChunks.end()) {
    iter->second->state = ChunkState::MarkedEmpty;
  }
}
void Surroundings::setFilled(Chunk *chunk) {
  if (auto iter = usedChunks.find(chunk); iter != usedChunks.end()) {
    iter->second->state = ChunkState::Filled;
  }
}
void Surroundings::moveSurroundings(SurrMoveDir direction) {
  auto tmp = partsMap;
  const auto directionVect = SurrDirToVec(direction);
  const auto areaSize = step * 30.f;
  const auto surroundingsStep = glm::vec3{size} * areaSize;
  using namespace MakeRange;
  for (unsigned int i : range(27)) {
    partsMap[i] = tmp[static_cast<int>(PosForDir(direction, static_cast<SurrPos>(i)))];
  }
  glm::vec3 newStartPosition;
  glm::vec3 newCenter;
  std::vector<SurrPos> forCoords;
  switch (direction) {
  case SurrMoveDir::Front:
    forCoords = {SurrPos::FrontLowLeft,  SurrPos::FrontLowMid,  SurrPos::FrontLowRight,
                 SurrPos::FrontMidLeft,  SurrPos::FrontMidMid,  SurrPos::FrontMidRight,
                 SurrPos::FrontHighLeft, SurrPos::FrontHighMid, SurrPos::FrontHighRight};
    break;
  case SurrMoveDir::Back:
    forCoords = {SurrPos::BackLowLeft,  SurrPos::BackLowMid,   SurrPos::BackLowRight, SurrPos::BackMidLeft,  SurrPos::BackMidMid,
                 SurrPos::BackMidRight, SurrPos::BackHighLeft, SurrPos::BackHighMid,  SurrPos::BackHighRight};

    break;
  case SurrMoveDir::Right:
    forCoords = {SurrPos::BackHighRight,  SurrPos::BackLowRight,  SurrPos::BackMidRight,
                 SurrPos::FrontHighRight, SurrPos::FrontLowRight, SurrPos::FrontMidRight,
                 SurrPos::MidHighRight,   SurrPos::MidLowRight,   SurrPos::MidMidRight};
    break;
  case SurrMoveDir::Left:
    forCoords = {SurrPos::BackHighLeft, SurrPos::BackLowLeft, SurrPos::BackMidLeft, SurrPos::FrontHighLeft, SurrPos::FrontLowLeft,
                 SurrPos::FrontMidLeft, SurrPos::MidHighLeft, SurrPos::MidLowLeft,  SurrPos::MidMidLeft};
    break;
  case SurrMoveDir::Down:
    forCoords = {SurrPos::MidLowLeft,  SurrPos::FrontLowLeft,  SurrPos::BackLowLeft, SurrPos::BackLowMid, SurrPos::BackLowRight,
                 SurrPos::FrontLowMid, SurrPos::FrontLowRight, SurrPos::MidLowMid,   SurrPos::MidLowRight};
    break;
  case SurrMoveDir::Up:
    forCoords = {SurrPos::MidHighLeft,    SurrPos::FrontHighLeft, SurrPos::BackHighLeft,
                 SurrPos::BackHighMid,    SurrPos::BackHighRight, SurrPos::FrontHighMid,
                 SurrPos::FrontHighRight, SurrPos::MidHighMid,    SurrPos::MidHighRight};
    break;
  }

  for (auto pos : forCoords) {
    newStartPosition =
        partsMap[static_cast<int>(CoordSourceForDir(direction, pos))]->startPosition + directionVect * surroundingsStep;
    newCenter = newStartPosition + surroundingsStep / 2.f;
    const auto tmp = partsMap[static_cast<int>(pos)]->restart(newStartPosition, newCenter, size, step, lodData);
    unused.insert(unused.end(), tmp.begin(), tmp.end());
  }
}
void Surroundings::rearrangeSurroundings(SurrPos newMid) {
  switch (newMid) {
  case SurrPos::BackLowLeft:
    moveSurroundings(SurrMoveDir::Back);
    moveSurroundings(SurrMoveDir::Down);
    moveSurroundings(SurrMoveDir::Left);
    break;
  case SurrPos::BackLowMid:
    moveSurroundings(SurrMoveDir::Back);
    moveSurroundings(SurrMoveDir::Down);
    break;
  case SurrPos::BackLowRight:
    moveSurroundings(SurrMoveDir::Back);
    moveSurroundings(SurrMoveDir::Down);
    moveSurroundings(SurrMoveDir::Right);
    break;
  case SurrPos::BackMidLeft:
    moveSurroundings(SurrMoveDir::Back);
    moveSurroundings(SurrMoveDir::Left);
    break;
  case SurrPos::BackMidMid:
    moveSurroundings(SurrMoveDir::Back);
    break;
  case SurrPos::BackMidRight:
    moveSurroundings(SurrMoveDir::Back);
    moveSurroundings(SurrMoveDir::Right);
    break;
  case SurrPos::BackHighLeft:
    moveSurroundings(SurrMoveDir::Back);
    moveSurroundings(SurrMoveDir::Up);
    moveSurroundings(SurrMoveDir::Left);
    break;
  case SurrPos::BackHighMid:
    moveSurroundings(SurrMoveDir::Back);
    moveSurroundings(SurrMoveDir::Up);
    break;
  case SurrPos::BackHighRight:
    moveSurroundings(SurrMoveDir::Back);
    moveSurroundings(SurrMoveDir::Up);
    moveSurroundings(SurrMoveDir::Right);
    break;
  case SurrPos::MidLowLeft:
    moveSurroundings(SurrMoveDir::Down);
    moveSurroundings(SurrMoveDir::Left);
    break;
  case SurrPos::MidLowMid:
    moveSurroundings(SurrMoveDir::Down);
    break;
  case SurrPos::MidLowRight:
    moveSurroundings(SurrMoveDir::Down);
    moveSurroundings(SurrMoveDir::Right);
    break;
  case SurrPos::MidMidLeft:
    moveSurroundings(SurrMoveDir::Left);
    break;
  case SurrPos::MidMidMid:
    break;
  case SurrPos::MidMidRight:
    moveSurroundings(SurrMoveDir::Right);
    break;
  case SurrPos::MidHighLeft:
    moveSurroundings(SurrMoveDir::Up);
    moveSurroundings(SurrMoveDir::Left);
    break;
  case SurrPos::MidHighMid:
    moveSurroundings(SurrMoveDir::Up);
    break;
  case SurrPos::MidHighRight:
    moveSurroundings(SurrMoveDir::Up);
    moveSurroundings(SurrMoveDir::Right);
    break;
  case SurrPos::FrontLowLeft:
    moveSurroundings(SurrMoveDir::Front);
    moveSurroundings(SurrMoveDir::Down);
    moveSurroundings(SurrMoveDir::Left);
    break;
  case SurrPos::FrontLowMid:
    moveSurroundings(SurrMoveDir::Front);
    moveSurroundings(SurrMoveDir::Down);
    break;
  case SurrPos::FrontLowRight:
    moveSurroundings(SurrMoveDir::Front);
    moveSurroundings(SurrMoveDir::Down);
    moveSurroundings(SurrMoveDir::Right);
    break;
  case SurrPos::FrontMidLeft:
    moveSurroundings(SurrMoveDir::Front);
    moveSurroundings(SurrMoveDir::Left);
    break;
  case SurrPos::FrontMidMid:
    moveSurroundings(SurrMoveDir::Front);
    break;
  case SurrPos::FrontMidRight:
    moveSurroundings(SurrMoveDir::Front);
    moveSurroundings(SurrMoveDir::Right);
    break;
  case SurrPos::FrontHighLeft:
    moveSurroundings(SurrMoveDir::Front);
    moveSurroundings(SurrMoveDir::Up);
    moveSurroundings(SurrMoveDir::Left);
    break;
  case SurrPos::FrontHighMid:
    moveSurroundings(SurrMoveDir::Front);
    moveSurroundings(SurrMoveDir::Up);
    break;
  case SurrPos::FrontHighRight:
    moveSurroundings(SurrMoveDir::Front);
    moveSurroundings(SurrMoveDir::Up);
    moveSurroundings(SurrMoveDir::Right);
    break;
  }
}
void Surroundings::checkForMapMove(glm::vec3 cameraPosition) {
  if (partsMap[static_cast<int>(SurrPos::MidMidMid)]->boundingBox.contains(cameraPosition)) {
    return;
  }
  using namespace MakeRange;
  SurrPos cameraIn = SurrPos::MidMidMid;
  for (unsigned int i : range(27)) {
    if (partsMap[i]->boundingBox.contains(cameraPosition)) {
      cameraIn = static_cast<SurrPos>(i);
      break;
    }
  }
  if (cameraIn == SurrPos::MidMidMid) {
    return;
  }
  rearrangeSurroundings(cameraIn);
}

bool Map::isInRange(glm::vec3 cameraPosition, float range) { return boundingSphere.distance(cameraPosition) <= range; }
void Map::init(glm::vec3 start, glm::vec3 center, glm::uvec3 tileSize, float step, const LODData &lodData) {
  using namespace MakeRange;
  startPosition = start;
  Map::center = center;
  boundingSphere = geo::BoundingSphere<3>{center, glm::distance(start, center)};
  boundingBox = geo::BoundingBox<3>(start, start + 32.f * step * glm::vec3{tileSize});
  const auto halfDist = 16.f * glm::vec3{step};

  Tile tileToCopy{ChunkState::NotLoaded, LOD{}, start + startPosition, center + startPosition};
  tileToCopy.lod.initTree(lodData.levelCount);

  for (auto i : range(tiles.size())) {
    uint x = i % tileSize.x;
    uint y = i / tileSize.x % tileSize.y;
    uint z = i / (tileSize.x * tileSize.y) % tileSize.z;
    const auto start = glm::vec3{x, y, z} * step * 30.f;
    const auto center = start + halfDist;
    tiles[i] = tileToCopy;
    tiles[i].pos = start + startPosition;
    tiles[i].center = center + startPosition;

    unsigned int lastLevel = 0;
    unsigned int perLevelCnt = 0;
    float stepForLevel = step;
    tiles[i].lod.tree.traverseBreadthFirst([this, &stepForLevel, start, &lastLevel, &perLevelCnt](LODTreeData &loddata) {
      if (loddata.level != lastLevel) {
        perLevelCnt = 0;
        lastLevel = loddata.level;
        stepForLevel /= 2;
      }
      const auto st = start + startPosition + stepForLevel * forLOD(perLevelCnt, LODData::lenForLevel(loddata.level)) * 30.f;
      const auto ctr = st + 15.f * stepForLevel;
      loddata.boundingSphere = geo::BoundingSphere<3>{ctr, glm::distance(st, ctr)};
      loddata.index = perLevelCnt;
      ++perLevelCnt;
    });
  }
}
std::vector<Chunk *> Map::restart(glm::vec3 start, glm::vec3 center, glm::uvec3 tileSize, float step, const LODData &lodData) {
  using namespace MakeRange;
  startPosition = start;
  Map::center = center;
  std::vector<Chunk *> result;
  boundingSphere = geo::BoundingSphere<3>{center, glm::distance(start, center)};
  boundingBox = geo::BoundingBox<3>(start, start + 32.f * step * glm::vec3{tileSize});
  const auto halfDist = 16.f * glm::vec3{step};

  for (auto i : range(tiles.size())) {
    tiles[i].lod.tree.traverseDepthFirstIf([&result](LODTreeData &loddata) {
      if (loddata.chunk != nullptr) {
        result.emplace_back(loddata.chunk);
        loddata.chunk = nullptr;
        loddata.isCurrent = false;
      }
      return loddata.isDivided;
    });
    uint x = i % tileSize.x;
    uint y = i / tileSize.x % tileSize.y;
    uint z = i / (tileSize.x * tileSize.y) % tileSize.z;
    const auto start = glm::vec3{x, y, z} * step * 30.f;
    const auto center = start + halfDist;
    tiles[i].state = ChunkState::NotLoaded;
    tiles[i].pos = start + startPosition;
    tiles[i].center = center + startPosition;

    unsigned int lastLevel = 0;
    unsigned int perLevelCnt = 0;
    float stepForLevel = step;
    tiles[i].lod.tree.traverseBreadthFirst([this, &stepForLevel, start, &lastLevel, &perLevelCnt](LODTreeData &loddata) {
      if (loddata.level != lastLevel) {
        perLevelCnt = 0;
        lastLevel = loddata.level;
        stepForLevel /= 2;
      }
      const auto st = start + startPosition + stepForLevel * forLOD(perLevelCnt, LODData::lenForLevel(loddata.level)) * 30.f;
      const auto ctr = st + 15.f * stepForLevel;
      loddata.boundingSphere = geo::BoundingSphere<3>{ctr, glm::distance(st, ctr)};
      loddata.index = perLevelCnt;
      loddata.isDivided = false;
      ++perLevelCnt;
    });
  }
  return result;
}
