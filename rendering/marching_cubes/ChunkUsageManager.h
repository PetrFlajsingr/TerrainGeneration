//
// Created by petr on 12/22/19.
//

#ifndef TERRAINGENERATION_CHUNKUSAGEMANAGER_H
#define TERRAINGENERATION_CHUNKUSAGEMANAGER_H

#include "Chunk.h"
#include "LODChunkController.h"
#include <condition_variable>
#include <list>
#include <mutex>
#include <vector>

enum class Unloading { Aggresive, Relaxed };

using ChunkPool = std::vector<Chunk>;
ChunkPool initPool(ChunkPool::size_type chunkCount);

class Tile;
struct ChunkUsageInitData {
  ChunkUsageInitData(std::size_t chunkPoolSize, std::size_t chunksPerFrameLimit, float loadingDistance, std::size_t LODlevelCount,
                     float chunkStep, Unloading unloading = Unloading::Aggresive);

  std::size_t chunkPoolSize;
  std::size_t chunksPerFrameLimit;
  float loadingDistance;
  Unloading unloading;
  std::size_t LODlevelCount;
  float chunkStep;
};

namespace detail {
//TODO
struct DefaultChunkBorrowingPolicy {

};

struct ThreadSafeChunkBorrowingPolicy {

};

}

//template <typename ChunkBorrowingPolicy = detail::DefaultChunkBorrowingPolicy>
class ChunkUsageManager {
  // list because of frequent deletion in random places
  using ChunkPtrs = std::list<Chunk *>;
  using ChunkToTileMap = std::unordered_map<Chunk *, Tile *>;

public:
  explicit ChunkUsageManager(ChunkUsageInitData initData);

  void newFrame(glm::vec3 cameraPosition);

  void manageTile(Tile &tile);

  [[nodiscard]] bool hasAvailable() const;
  [[nodiscard]] Chunk *borrowChunk();
  [[nodiscard]] Chunk *borrowChunk(Tile &tile);

  void returnChunk(Chunk *chunk);
  void returnTileChunk(Chunk *chunk);

  const ChunkPtrs &getUsedChunks() { return data.used; }

  [[nodiscard]] const ChunkToTileMap &getChunkToTileMap() { return data.chunkToTileMap; }

  struct Info {
    std::size_t availableChunks;
    std::size_t usedChunks;
    Unloading unloadingPolicy;
  };

  [[nodiscard]] Info getInfo() const { return {data.available.size(), data.used.size(), unloading}; }
  [[nodiscard]] const LODChunkController::Counters &getCounters() const { return lodController.getCounters(); }

private:
  ChunkPool chunkPool;
  std::size_t chunksPerFrameLimit;
  Unloading unloading;
  float loadingDistance;
  glm::vec3 cameraPosition{0, 0, 0};

  std::mutex mtx;

  LODChunkController lodController;
  struct {
    ChunkPtrs available;
    ChunkPtrs used;
    // for fast setting of chunk status after computation
    ChunkToTileMap chunkToTileMap;
  } data;

  void manageUnloadedTile(Tile &tile);
  void manageFilledTile(Tile &tile);
  void manageMarkedEmptyTile(Tile &tile);
};

#endif // TERRAINGENERATION_CHUNKUSAGEMANAGER_H