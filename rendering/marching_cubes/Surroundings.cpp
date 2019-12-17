//
// Created by petr on 11/22/19.
//

#include "Surroundings.h"
#include "io/print.h"
#include "utils/types/Zip.h"
#include <error_handling/exceptions.h>

using namespace MakeRange;

Surroundings::Surroundings(float loadDistance, glm::uvec3 size,
                           unsigned int chunkPoolSize, float step)
    : loadDistance(loadDistance), size(size), step(step) {
  for (auto &map : maps) {
    map.tiles.resize(size.x * size.y * size.z);
  }
  for ([[maybe_unused]] auto _ : range(chunkPoolSize)) {
    chunkPool.emplace_back(glm::vec3{-30, -30, -30}, step, 32);
  }

  const auto halfDist = 16.f * glm::vec3{step} * glm::vec3{size};

  using namespace MakeRange;
  const auto areaSize = step * 30.f;
  for (auto [index, coords] :
       zip(range(27), range<float, 3>({-1, -1, -1}, {2, 2, 2}, {1, 1, 1}))) {
    const auto [x, y, z] = coords;
    const auto startPos = glm::vec3{x, y, z} * glm::vec3{size} * areaSize;
    maps[index].init(startPos, startPos + halfDist, size, step);
    partsMap[static_cast<SurrPos>(index)] = &maps[index];
  }
  for (auto &chunk : chunkPool) {
    available.emplace_back(&chunk);
  }
}
std::list<Chunk *> Surroundings::getForCompute(glm::vec3 position) {
  checkForMapMove(position);
  for (auto ptr : unused) {
    available.emplace_back(ptr);
    usedChunks[ptr] = nullptr;
    used.remove(ptr);
  }
  unused.clear();
  //  TODO: move to GPU for speedup
  uint availableCount = available.size();
  uint emptyCount = 0;
  uint setupCount = 0;
  uint filledCount = 0;
  uint notLoadedCount = 0;
  constexpr uint availableThreshold = 30;
  unsigned int mapCnt = 0;
  for (auto &map : maps) {
    if (!map.isInRange(position, loadDistance)) {
      continue;
    }
    ++mapCnt;

    for (auto &tile : map.tiles) {
      if (tile.state == ChunkState::NotLoaded) {
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

          tile.state = ChunkState::Setup;
          tile.ptr = chunk;
          ++setupCount;
        } else {
          ++notLoadedCount;
        }
      } else if (tile.state == ChunkState::Filled) {
        if ((aggressiveChunkUnloading || availableCount < availableThreshold) &&
            tile.ptr->boundingSphere.distance(position) > loadDistance) {
          used.remove(tile.ptr);

          usedChunks[tile.ptr] = nullptr;

          available.emplace_back(tile.ptr);
          ++availableCount;
          ++notLoadedCount;
          tile.state = ChunkState::NotLoaded;
          tile.ptr = nullptr;
        } else {
          ++filledCount;
        }
      } else if (tile.state == ChunkState::Empty) {
        if (tile.ptr != nullptr) {
          available.emplace_back(tile.ptr);
          used.remove(tile.ptr);

          usedChunks[tile.ptr] = nullptr;

          ++availableCount;
          tile.ptr = nullptr;
        }
        ++emptyCount;
      } else if (tile.state == ChunkState::Setup) {
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
    iter->second->state = ChunkState::Empty;
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
    partsMap[static_cast<SurrPos>(i)] =
        tmp[PosForDir(direction, static_cast<SurrPos>(i))];
  }
  glm::vec3 newStartPosition;
  glm::vec3 newCenter;
  std::vector<SurrPos> forCoords;
  switch (direction) {
  case SurrMoveDir::Front:
    forCoords = {SurrPos::FrontLowLeft,  SurrPos::FrontLowMid,
                 SurrPos::FrontLowRight, SurrPos::FrontMidLeft,
                 SurrPos::FrontMidMid,   SurrPos::FrontMidRight,
                 SurrPos::FrontHighLeft, SurrPos::FrontHighMid,
                 SurrPos::FrontHighRight};
    break;
  case SurrMoveDir::Back:
    forCoords = {
        SurrPos::BackLowLeft,  SurrPos::BackLowMid,  SurrPos::BackLowRight,
        SurrPos::BackMidLeft,  SurrPos::BackMidMid,  SurrPos::BackMidRight,
        SurrPos::BackHighLeft, SurrPos::BackHighMid, SurrPos::BackHighRight};

    break;
  case SurrMoveDir::Right:
    forCoords = {SurrPos::BackHighRight, SurrPos::BackLowRight,
                 SurrPos::BackMidRight,  SurrPos::FrontHighRight,
                 SurrPos::FrontLowRight, SurrPos::FrontMidRight,
                 SurrPos::MidHighRight,  SurrPos::MidLowRight,
                 SurrPos::MidMidRight};
    break;
  case SurrMoveDir::Left:
    forCoords = {
        SurrPos::BackHighLeft,  SurrPos::BackLowLeft,  SurrPos::BackMidLeft,
        SurrPos::FrontHighLeft, SurrPos::FrontLowLeft, SurrPos::FrontMidLeft,
        SurrPos::MidHighLeft,   SurrPos::MidLowLeft,   SurrPos::MidMidLeft};
    break;
  case SurrMoveDir::Down:
    forCoords = {
        SurrPos::MidLowLeft,    SurrPos::FrontLowLeft, SurrPos::BackLowLeft,
        SurrPos::BackLowMid,    SurrPos::BackLowRight, SurrPos::FrontLowMid,
        SurrPos::FrontLowRight, SurrPos::MidLowMid,    SurrPos::MidLowRight};
    break;
  case SurrMoveDir::Up:
    forCoords = {SurrPos::MidHighLeft,    SurrPos::FrontHighLeft,
                 SurrPos::BackHighLeft,   SurrPos::BackHighMid,
                 SurrPos::BackHighRight,  SurrPos::FrontHighMid,
                 SurrPos::FrontHighRight, SurrPos::MidHighMid,
                 SurrPos::MidHighRight};
    break;
  }

  for (auto pos : forCoords) {
    newStartPosition =
        partsMap[CoordSourceForDir(direction, pos)]->startPosition +
            directionVect * surroundingsStep;
    newCenter = newStartPosition + surroundingsStep / 2.f;
    const auto tmp = partsMap[pos]->init(newStartPosition, newCenter, size, step);
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
  if (partsMap[SurrPos::MidMidMid]->boundingBox.contains(cameraPosition)) {
    return;
  }
  using namespace MakeRange;
  SurrPos cameraIn = SurrPos::MidMidMid;
  for (unsigned int i : range(27)) {
    if (partsMap[static_cast<SurrPos>(i)]->boundingBox.contains(
        cameraPosition)) {
      cameraIn = static_cast<SurrPos>(i);
      break;
    }
  }
  if (cameraIn == SurrPos::MidMidMid) {
    return;
  }
  rearrangeSurroundings(cameraIn);
}
glm::vec3 SurrDirToVec(SurrMoveDir direction) {
  switch (direction) {
  case SurrMoveDir::Front:
    return glm::vec3{0, 0, 1};
  case SurrMoveDir::Back:
    return glm::vec3{0, 0, -1};
  case SurrMoveDir::Left:
    return glm::vec3{-1, 0, 0};
  case SurrMoveDir::Right:
    return glm::vec3{1, 0, 0};
  case SurrMoveDir::Up:
    return glm::vec3{0, 1, 0};
  case SurrMoveDir::Down:
    return glm::vec3{0, -1, 0};
  }
}

SurrPos PosForDir(SurrMoveDir direction, SurrPos pos) {
  switch (direction) {
  case SurrMoveDir::Front:
    switch (pos) {
    case SurrPos::BackLowLeft: return SurrPos::MidLowLeft;
    case SurrPos::BackLowMid: return SurrPos::MidLowMid;
    case SurrPos::BackLowRight: return SurrPos::MidLowRight;
    case SurrPos::BackMidLeft: return SurrPos::MidMidLeft;
    case SurrPos::BackMidMid: return SurrPos::MidMidMid;
    case SurrPos::BackMidRight: return SurrPos::MidMidRight;
    case SurrPos::BackHighLeft: return SurrPos::MidHighLeft;
    case SurrPos::BackHighMid: return SurrPos::MidHighMid;
    case SurrPos::BackHighRight: return SurrPos::MidHighRight;
    case SurrPos::MidLowLeft: return SurrPos::FrontLowLeft;
    case SurrPos::MidLowMid: return SurrPos::FrontLowMid;
    case SurrPos::MidLowRight: return SurrPos::FrontLowRight;
    case SurrPos::MidMidLeft: return SurrPos::FrontMidLeft;
    case SurrPos::MidMidMid: return SurrPos::FrontMidMid;
    case SurrPos::MidMidRight: return SurrPos::FrontMidRight;
    case SurrPos::MidHighLeft: return SurrPos::FrontHighLeft;
    case SurrPos::MidHighMid: return SurrPos::FrontHighMid;
    case SurrPos::MidHighRight: return SurrPos::FrontHighRight;
    case SurrPos::FrontLowLeft: return SurrPos::BackLowLeft;
    case SurrPos::FrontLowMid: return SurrPos::BackLowMid;
    case SurrPos::FrontLowRight: return SurrPos::BackLowRight;
    case SurrPos::FrontMidLeft: return SurrPos::BackMidLeft;
    case SurrPos::FrontMidMid: return SurrPos::BackMidMid;
    case SurrPos::FrontMidRight: return SurrPos::BackMidRight;
    case SurrPos::FrontHighLeft: return SurrPos::BackHighLeft;
    case SurrPos::FrontHighMid: return SurrPos::BackHighMid;
    case SurrPos::FrontHighRight: return SurrPos::BackHighRight;
    }
  case SurrMoveDir::Back:
    switch (pos) {
    case SurrPos::BackLowLeft: return SurrPos::FrontLowLeft;
    case SurrPos::BackLowMid: return SurrPos::FrontLowMid;
    case SurrPos::BackLowRight: return SurrPos::FrontLowRight;
    case SurrPos::BackMidLeft: return SurrPos::FrontMidLeft;
    case SurrPos::BackMidMid: return SurrPos::FrontMidMid;
    case SurrPos::BackMidRight: return SurrPos::FrontMidRight;
    case SurrPos::BackHighLeft: return SurrPos::FrontHighLeft;
    case SurrPos::BackHighMid: return SurrPos::FrontHighMid;
    case SurrPos::BackHighRight: return SurrPos::FrontHighRight;
    case SurrPos::MidLowLeft: return SurrPos::BackLowLeft;
    case SurrPos::MidLowMid: return SurrPos::BackLowMid;
    case SurrPos::MidLowRight: return SurrPos::BackLowRight;
    case SurrPos::MidMidLeft: return SurrPos::BackMidLeft;
    case SurrPos::MidMidMid: return SurrPos::BackMidMid;
    case SurrPos::MidMidRight: return SurrPos::BackMidRight;
    case SurrPos::MidHighLeft: return SurrPos::BackHighLeft;
    case SurrPos::MidHighMid: return SurrPos::BackHighMid;
    case SurrPos::MidHighRight: return SurrPos::BackHighRight;
    case SurrPos::FrontLowLeft: return SurrPos::MidLowLeft;
    case SurrPos::FrontLowMid: return SurrPos::MidLowMid;
    case SurrPos::FrontLowRight: return SurrPos::MidLowRight;
    case SurrPos::FrontMidLeft: return SurrPos::MidMidLeft;
    case SurrPos::FrontMidMid: return SurrPos::MidMidMid;
    case SurrPos::FrontMidRight: return SurrPos::MidMidRight;
    case SurrPos::FrontHighLeft: return SurrPos::MidHighLeft;
    case SurrPos::FrontHighMid: return SurrPos::MidHighMid;
    case SurrPos::FrontHighRight: return SurrPos::MidHighRight;
    }
  case SurrMoveDir::Right:
    switch (pos) {
    case SurrPos::BackHighLeft:return SurrPos::BackHighMid;
    case SurrPos::BackLowLeft:return SurrPos::BackLowMid;
    case SurrPos::BackMidLeft:return SurrPos::BackMidMid;
    case SurrPos::FrontHighLeft:return SurrPos::FrontHighMid;
    case SurrPos::FrontLowLeft:return SurrPos::FrontLowMid;
    case SurrPos::FrontMidLeft:return SurrPos::FrontMidMid;
    case SurrPos::MidHighLeft:return SurrPos::MidHighMid;
    case SurrPos::MidLowLeft:return SurrPos::MidLowMid;
    case SurrPos::MidMidLeft:return SurrPos::MidMidMid;
    case SurrPos::BackHighMid: return SurrPos::BackHighRight;
    case SurrPos::BackLowMid: return SurrPos::BackLowRight;
    case SurrPos::BackMidMid: return SurrPos::BackMidRight;
    case SurrPos::FrontHighMid: return SurrPos::FrontHighRight;
    case SurrPos::FrontLowMid: return SurrPos::FrontLowRight;
    case SurrPos::FrontMidMid: return SurrPos::FrontMidRight;
    case SurrPos::MidHighMid: return SurrPos::MidHighRight;
    case SurrPos::MidLowMid: return SurrPos::MidLowRight;
    case SurrPos::MidMidMid: return SurrPos::MidMidRight;
    case SurrPos::BackHighRight:return SurrPos::BackHighLeft;
    case SurrPos::BackLowRight:return SurrPos::BackLowLeft;
    case SurrPos::BackMidRight:return SurrPos::BackMidLeft;
    case SurrPos::FrontHighRight:return SurrPos::FrontHighLeft;
    case SurrPos::FrontLowRight:return SurrPos::FrontLowLeft;
    case SurrPos::FrontMidRight:return SurrPos::FrontMidLeft;
    case SurrPos::MidHighRight:return SurrPos::MidHighLeft;
    case SurrPos::MidLowRight:return SurrPos::MidLowLeft;
    case SurrPos::MidMidRight:return SurrPos::MidMidLeft;
    }
  case SurrMoveDir::Left:
    switch (pos) {
    case SurrPos::BackHighRight: return SurrPos::BackHighMid;
    case SurrPos::BackLowRight: return SurrPos::BackLowMid;
    case SurrPos::BackMidRight: return SurrPos::BackMidMid;
    case SurrPos::FrontHighRight: return SurrPos::FrontHighMid;
    case SurrPos::FrontLowRight: return SurrPos::FrontLowMid;
    case SurrPos::FrontMidRight: return SurrPos::FrontMidMid;
    case SurrPos::MidHighRight: return SurrPos::MidHighMid;
    case SurrPos::MidLowRight: return SurrPos::MidLowMid;
    case SurrPos::MidMidRight: return SurrPos::MidMidMid;
    case SurrPos::BackHighMid: return SurrPos::BackHighLeft;
    case SurrPos::BackLowMid: return SurrPos::BackLowLeft;
    case SurrPos::BackMidMid: return SurrPos::BackMidLeft;
    case SurrPos::FrontHighMid: return SurrPos::FrontHighLeft;
    case SurrPos::FrontLowMid: return SurrPos::FrontLowLeft;
    case SurrPos::FrontMidMid: return SurrPos::FrontMidLeft;
    case SurrPos::MidHighMid: return SurrPos::MidHighLeft;
    case SurrPos::MidLowMid: return SurrPos::MidLowLeft;
    case SurrPos::MidMidMid: return SurrPos::MidMidLeft;
    case SurrPos::BackHighLeft: return SurrPos::BackHighRight;
    case SurrPos::BackLowLeft: return SurrPos::BackLowRight;
    case SurrPos::BackMidLeft: return SurrPos::BackMidRight;
    case SurrPos::FrontHighLeft: return SurrPos::FrontHighRight;
    case SurrPos::FrontLowLeft: return SurrPos::FrontLowRight;
    case SurrPos::FrontMidLeft: return SurrPos::FrontMidRight;
    case SurrPos::MidHighLeft: return SurrPos::MidHighRight;
    case SurrPos::MidLowLeft: return SurrPos::MidLowRight;
    case SurrPos::MidMidLeft: return SurrPos::MidMidRight;
    }
  case SurrMoveDir::Down:
    switch (pos) {
    case SurrPos::MidHighLeft: return SurrPos::MidMidLeft;
    case SurrPos::FrontHighLeft: return SurrPos::FrontMidLeft;
    case SurrPos::BackHighLeft: return SurrPos::BackMidLeft;
    case SurrPos::BackHighMid: return SurrPos::BackMidMid;
    case SurrPos::BackHighRight: return SurrPos::BackMidRight;
    case SurrPos::FrontHighMid: return SurrPos::FrontMidMid;
    case SurrPos::FrontHighRight: return SurrPos::FrontMidRight;
    case SurrPos::MidHighMid: return SurrPos::MidMidMid;
    case SurrPos::MidHighRight: return SurrPos::MidMidRight;
    case SurrPos::MidMidLeft: return SurrPos::MidLowLeft;
    case SurrPos::FrontMidLeft: return SurrPos::FrontLowLeft;
    case SurrPos::BackMidLeft: return SurrPos::BackLowLeft;
    case SurrPos::BackMidMid: return SurrPos::BackLowMid;
    case SurrPos::BackMidRight: return SurrPos::BackLowRight;
    case SurrPos::FrontMidMid: return SurrPos::FrontLowMid;
    case SurrPos::FrontMidRight: return SurrPos::FrontLowRight;
    case SurrPos::MidMidMid: return SurrPos::MidLowMid;
    case SurrPos::MidMidRight: return SurrPos::MidLowRight;
    case SurrPos::MidLowLeft: return SurrPos::MidHighLeft;
    case SurrPos::FrontLowLeft: return SurrPos::FrontHighLeft;
    case SurrPos::BackLowLeft: return SurrPos::BackHighLeft;
    case SurrPos::BackLowMid: return SurrPos::BackHighMid;
    case SurrPos::BackLowRight: return SurrPos::BackHighRight;
    case SurrPos::FrontLowMid: return SurrPos::FrontHighMid;
    case SurrPos::FrontLowRight: return SurrPos::FrontHighRight;
    case SurrPos::MidLowMid: return SurrPos::MidHighMid;
    case SurrPos::MidLowRight: return SurrPos::MidHighRight;
    }
  case SurrMoveDir::Up:
    switch(pos) {
      case SurrPos::MidHighLeft: return SurrPos::MidLowLeft;
      case SurrPos::FrontHighLeft: return SurrPos::FrontLowLeft;
      case SurrPos::BackHighLeft: return SurrPos::BackLowLeft;
      case SurrPos::BackHighMid: return SurrPos::BackLowMid;
      case SurrPos::BackHighRight: return SurrPos::BackLowRight;
      case SurrPos::FrontHighMid: return SurrPos::FrontLowMid;
      case SurrPos::FrontHighRight: return SurrPos::FrontLowRight;
      case SurrPos::MidHighMid: return SurrPos::MidLowMid;
      case SurrPos::MidHighRight: return SurrPos::MidLowRight;

      case SurrPos::MidMidLeft: return SurrPos::MidHighLeft;
      case SurrPos::FrontMidLeft: return SurrPos::FrontHighLeft;
      case SurrPos::BackMidLeft: return SurrPos::BackHighLeft;
      case SurrPos::BackMidMid: return SurrPos::BackHighMid;
      case SurrPos::BackMidRight: return SurrPos::BackHighRight;
      case SurrPos::FrontMidMid: return SurrPos::FrontHighMid;
      case SurrPos::FrontMidRight: return SurrPos::FrontHighRight;
      case SurrPos::MidMidMid: return SurrPos::MidHighMid;
      case SurrPos::MidMidRight: return SurrPos::MidHighRight;

      case SurrPos::MidLowLeft: return SurrPos::MidMidLeft;
      case SurrPos::FrontLowLeft: return SurrPos::FrontMidLeft;
      case SurrPos::BackLowLeft: return SurrPos::BackMidLeft;
      case SurrPos::BackLowMid: return SurrPos::BackMidMid;
      case SurrPos::BackLowRight: return SurrPos::BackMidRight;
      case SurrPos::FrontLowMid: return SurrPos::FrontMidMid;
      case SurrPos::FrontLowRight: return SurrPos::FrontMidRight;
      case SurrPos::MidLowMid: return SurrPos::MidMidMid;
      case SurrPos::MidLowRight: return SurrPos::MidMidRight;
    }
  }
  throw exc::InternalError("Invalid input.");
}
SurrPos CoordSourceForDir(SurrMoveDir direction, SurrPos pos) {
  switch (direction) {
  case SurrMoveDir::Front:
    switch (pos) {
    case SurrPos::FrontLowLeft:
      return SurrPos::MidLowLeft;
    case SurrPos::FrontLowMid:
      return SurrPos::MidLowMid;
    case SurrPos::FrontLowRight:
      return SurrPos::MidLowRight;
    case SurrPos::FrontMidLeft:
      return SurrPos::MidMidLeft;
    case SurrPos::FrontMidMid:
      return SurrPos::MidMidMid;
    case SurrPos::FrontMidRight:
      return SurrPos::MidMidRight;
    case SurrPos::FrontHighLeft:
      return SurrPos::MidHighLeft;
    case SurrPos::FrontHighMid:
      return SurrPos::MidHighMid;
    case SurrPos::FrontHighRight:
      return SurrPos::MidHighRight;
    }
  case SurrMoveDir::Back:
    switch (pos) {
    case SurrPos::BackLowLeft: return SurrPos::MidLowLeft;
    case SurrPos::BackLowMid: return SurrPos::MidLowMid;
    case SurrPos::BackLowRight: return SurrPos::MidLowRight;
    case SurrPos::BackMidLeft: return SurrPos::MidMidLeft;
    case SurrPos::BackMidMid: return SurrPos::MidMidMid;
    case SurrPos::BackMidRight: return SurrPos::MidMidRight;
    case SurrPos::BackHighLeft: return SurrPos::MidHighLeft;
    case SurrPos::BackHighMid: return SurrPos::MidHighMid;
    case SurrPos::BackHighRight: return SurrPos::MidHighRight;
    }
  case SurrMoveDir::Right:
    switch (pos) {
    case SurrPos::BackHighRight: return SurrPos::BackHighMid;
    case SurrPos::BackLowRight: return SurrPos::BackLowMid;
    case SurrPos::BackMidRight: return SurrPos::BackMidMid;
    case SurrPos::FrontHighRight: return SurrPos::FrontHighMid;
    case SurrPos::FrontLowRight: return SurrPos::FrontLowMid;
    case SurrPos::FrontMidRight: return SurrPos::FrontMidMid;
    case SurrPos::MidHighRight: return SurrPos::MidHighMid;
    case SurrPos::MidLowRight: return SurrPos::MidLowMid;
    case SurrPos::MidMidRight: return SurrPos::MidMidMid;
    }
  case SurrMoveDir::Left:
    switch (pos) {
      case SurrPos::BackHighLeft: return SurrPos::BackHighMid;
      case SurrPos::BackLowLeft: return SurrPos::BackLowMid;
      case SurrPos::BackMidLeft: return SurrPos::BackMidMid;
      case SurrPos::FrontHighLeft: return SurrPos::FrontHighMid;
      case SurrPos::FrontLowLeft: return SurrPos::FrontLowMid;
      case SurrPos::FrontMidLeft: return SurrPos::FrontMidMid;
      case SurrPos::MidHighLeft: return SurrPos::MidHighMid;
      case SurrPos::MidLowLeft: return SurrPos::MidLowMid;
      case SurrPos::MidMidLeft: return SurrPos::MidMidMid;
    }
  case SurrMoveDir::Down:
    switch (pos) {
    case SurrPos::MidLowLeft: return SurrPos::MidMidLeft;
    case SurrPos::FrontLowLeft: return SurrPos::FrontMidLeft;
    case SurrPos::BackLowLeft: return SurrPos::BackMidLeft;
    case SurrPos::BackLowMid: return SurrPos::BackMidMid;
    case SurrPos::BackLowRight: return SurrPos::BackMidRight;
    case SurrPos::FrontLowMid: return SurrPos::FrontMidMid;
    case SurrPos::FrontLowRight: return SurrPos::FrontMidRight;
    case SurrPos::MidLowMid: return SurrPos::MidMidMid;
    case SurrPos::MidLowRight: return SurrPos::MidMidRight;
    }
  case SurrMoveDir::Up:
    switch (pos) {
    case SurrPos::MidHighLeft: return SurrPos::MidMidLeft;
    case SurrPos::FrontHighLeft: return SurrPos::FrontMidLeft;
    case SurrPos::BackHighLeft: return SurrPos::BackMidLeft;
    case SurrPos::BackHighMid: return SurrPos::BackMidMid;
    case SurrPos::BackHighRight: return SurrPos::BackMidRight;
    case SurrPos::FrontHighMid: return SurrPos::FrontMidMid;
    case SurrPos::FrontHighRight: return SurrPos::FrontMidRight;
    case SurrPos::MidHighMid: return SurrPos::MidMidMid;
    case SurrPos::MidHighRight: return SurrPos::MidMidRight;
    }
  }
  throw exc::InternalError("Invalid input.");
}
bool Map::isInRange(glm::vec3 cameraPosition, float range) {
  return boundingSphere.distance(cameraPosition) <= range;
}
std::vector<Chunk *> Map::init(glm::vec3 start, glm::vec3 center,
                               glm::uvec3 tileSize, float step) {
  using namespace MakeRange;
  startPosition = start;
  Map::center = center;
  std::vector<Chunk *> result;
  boundingSphere =
      geo::BoundingSphere<3>{center, glm::distance(start, center)};
  boundingBox =
      geo::BoundingBox<3>(start, start + 32.f * step * glm::vec3{tileSize});
  const auto halfDist = 16.f * glm::vec3{step};
  for (auto i : range(tiles.size())) {
    uint x = i % tileSize.x;
    uint y = i / tileSize.x % tileSize.y;
    uint z = i / (tileSize.x * tileSize.y) % tileSize.z;
    const auto start = glm::vec3{x, y, z} * step * 30.f;
    const auto center = start + halfDist;
    if (tiles[i].ptr != nullptr) {
      result.emplace_back(tiles[i].ptr);
    }
    tiles[i] = {ChunkState::NotLoaded, nullptr, start + startPosition,
                center + startPosition};
  }
  return result;
}
