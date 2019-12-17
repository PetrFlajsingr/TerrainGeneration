//
// Created by petr on 12/17/19.
//

#ifndef TERRAINGENERATION_TEMPVIEWPORTSETTER_H
#define TERRAINGENERATION_TEMPVIEWPORTSETTER_H

#include <array>
#include "geGL/StaticCalls.h"

class TempViewportSetter final {
public:
  explicit TempViewportSetter(const std::array<GLint, 4> &newViewport);
  ~TempViewportSetter();

private:
  std::array<GLint, 4> originalViewPort;
};

#endif // TERRAINGENERATION_TEMPVIEWPORTSETTER_H
