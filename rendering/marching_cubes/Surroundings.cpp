//
// Created by petr on 11/22/19.
//

#include "Surroundings.h"
#include "io/print.h"
#include "utils/types/Zip.h"
#include <error_handling/exceptions.h>

using namespace MakeRange;

Surroundings::Surroundings(float loadDistance, glm::uvec3 size, unsigned int chunkPoolSize, float step)
    : loadDistance(loadDistance), size(size), step(step), lodData(2, loadDistance, step),
      chunkUsageManager(ChunkUsageInitData{chunkPoolSize, 100, loadDistance, 2, step, Unloading::Aggresive}) {
  for (auto &map : maps) {
    map.tiles.resize(size.x * size.y * size.z);
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
}

std::list<Chunk *> Surroundings::getForCompute(glm::vec3 position) {
  checkForMapMove(position);
  for (auto ptr : unused) {
    chunkUsageManager.returnTileChunk(ptr);
  }
  unused.clear();

  chunkUsageManager.newFrame(position);
  for (auto &map : maps) {
    if (!map.isInRange(position, loadDistance)) {
      continue;
    }
    for (auto &tile : map.tiles) {
      chunkUsageManager.manageTile(tile);
    }
  }
  auto &used = chunkUsageManager.getUsedChunks();
  auto chunkUsageInfo = chunkUsageManager.getInfo();
  auto &counters = chunkUsageManager.getCounters();

  info = WString::Format(L"Chunks: available {}, used {}, setup {}, "
                         L"notLoaded {}",
                         chunkUsageInfo.availableChunks, chunkUsageInfo.usedChunks, counters.setupCount, counters.notLoadedCount);
  return used;
}
void Surroundings::setEmpty(Chunk *chunk) {
  auto &usedChunks = chunkUsageManager.getChunkToTileMap();
  if (auto iter = usedChunks.find(chunk); iter != usedChunks.end()) {
    iter->second->state = ChunkState::MarkedEmpty;
  }
}
void Surroundings::setFilled(Chunk *chunk) {
  auto &usedChunks = chunkUsageManager.getChunkToTileMap();
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
      const auto st =
          start + startPosition +
          stepForLevel * LODChunkController::offsetForSubChunk(perLevelCnt, LODData::ChunkCountInRow(loddata.level)) * 30.f;
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
      const auto st =
          start + startPosition +
          stepForLevel * LODChunkController::offsetForSubChunk(perLevelCnt, LODData::ChunkCountInRow(loddata.level)) * 30.f;
      const auto ctr = st + 15.f * stepForLevel;
      loddata.boundingSphere = geo::BoundingSphere<3>{ctr, glm::distance(st, ctr)};
      loddata.index = perLevelCnt;
      loddata.isDivided = false;
      ++perLevelCnt;
    });
  }
  return result;
}
