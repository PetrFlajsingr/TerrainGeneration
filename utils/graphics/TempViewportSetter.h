//
// Created by petr on 12/17/19.
//

#ifndef TERRAINGENERATION_TEMPVIEWPORTSETTER_H
#define TERRAINGENERATION_TEMPVIEWPORTSETTER_H

#include "geGL/StaticCalls.h"
#include <array>

class TempViewportSetter final {
public:
  explicit TempViewportSetter(const std::array<GLint, 4> &newViewport);
  ~TempViewportSetter();

private:
  std::array<GLint, 4> originalViewPort;
};

#endif // TERRAINGENERATION_TEMPVIEWPORTSETTER_H
