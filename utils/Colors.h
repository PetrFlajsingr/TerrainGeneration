//
// Created by petr on 12/19/19.
//

#ifndef TERRAINGENERATION_COLORS_H
#define TERRAINGENERATION_COLORS_H
#include <glm/glm.hpp>

namespace Color {
const glm::vec4 white{1, 1, 1, 1};
const glm::vec4 red{1, 0, 0, 1};
const glm::vec4 green{0, 1, 0, 1};
const glm::vec4 blue{0, 0, 1, 1};

inline glm::vec4 transparent(glm::vec4 color, float transparency) {
  assert(transparency >= 0.f && transparency <= 1.f);
  return {color.r, color.g, color.b, transparency};
}
}

#endif // TERRAINGENERATION_COLORS_H
