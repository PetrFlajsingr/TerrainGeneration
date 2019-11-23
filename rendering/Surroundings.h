//
// Created by petr on 11/22/19.
//

#ifndef TERRAINGENERATION_SURROUNDINGS_H
#define TERRAINGENERATION_SURROUNDINGS_H

#include "Chunk.h"
#include <String.h>
#include <glm/glm.hpp>
#include <list>
#include <observable/value.hpp>
#include <vector>

enum class ChunkIn { NotLoaded, Setup, Empty, Filled };

class Surroundings {
  struct Tile {
    ChunkIn state;
    Chunk *ptr;
    glm::vec3 pos;
    glm::vec3 center;
  };
  bool aggressiveChunkUnloading = true;

public:
  Surroundings(float loadDistance, glm::uvec3 size, uint chunkPoolSize,
               float step);

  observable::value<WString> info;

  std::list<Chunk *> getForCompute(glm::vec3 position);

  void setEmpty(Chunk *chunk);

  void setFilled(Chunk *chunk);

  const float loadDistance;
  const glm::uvec3 size;
  const float step;

private:
  std::vector<Tile> map;
  std::vector<Chunk> chunkPool;
  std::list<Chunk *> available;
  std::list<Chunk *> used;
};

#endif // TERRAINGENERATION_SURROUNDINGS_H
