//
// Created by petr on 12/17/19.
//

#ifndef TERRAINGENERATION_SURROUNDINGSENUMS_H
#define TERRAINGENERATION_SURROUNDINGSENUMS_H

#include <glm/glm.hpp>

enum class ChunkState { NotLoaded, Setup, MarkedEmpty, Filled, Empty };

enum class SurrMoveDir : unsigned int { Front, Back, Left, Right, Up, Down };

glm::vec3 SurrDirToVec(SurrMoveDir direction);

unsigned int SurrDirToOffset(SurrMoveDir direction);

enum class SurrPos : unsigned int {
  BackLowLeft = 0,
  BackLowMid,
  BackLowRight,
  BackMidLeft,
  BackMidMid,
  BackMidRight,
  BackHighLeft,
  BackHighMid,
  BackHighRight,

  MidLowLeft,
  MidLowMid,
  MidLowRight,
  MidMidLeft,
  MidMidMid,
  MidMidRight,
  MidHighLeft,
  MidHighMid,
  MidHighRight,

  FrontLowLeft,
  FrontLowMid,
  FrontLowRight,
  FrontMidLeft,
  FrontMidMid,
  FrontMidRight,
  FrontHighLeft,
  FrontHighMid,
  FrontHighRight,
};

SurrPos PosForDir(SurrMoveDir direction, SurrPos pos);
SurrPos CoordSourceForDir(SurrMoveDir direction, SurrPos pos);

#endif // TERRAINGENERATION_SURROUNDINGSENUMS_H
