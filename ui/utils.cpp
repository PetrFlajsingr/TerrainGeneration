//
// Created by petr on 11/20/19.
//

#include "utils.h"
#include <experimental/array>

std::array<glm::vec3, 4> sdlRectToGLCoordinates(const SDL_Rect &rect,
                                                unsigned int screenWidth,
                                                unsigned int screenHeight) {
  constexpr float normaliseVal = -1.f;
  float xLeft = static_cast<float>(rect.x) / static_cast<float>(screenWidth) * 2;
  float xRight = static_cast<float>(rect.x + rect.w) / static_cast<float>(screenWidth) * 2;
  float yTop = static_cast<float>(rect.y) / static_cast<float>(screenHeight) * 2;
  float yBottom = static_cast<float>(rect.y + rect.h) / static_cast<float>(screenHeight) * 2;
  return std::experimental::make_array(
      glm::vec3{xRight + normaliseVal, -(yTop + normaliseVal), 0.f},
      glm::vec3{xLeft + normaliseVal, -(yTop + normaliseVal), 0.f},
      glm::vec3{xRight + normaliseVal, -(yBottom + normaliseVal), 0.f},
      glm::vec3{xLeft + normaliseVal, -(yBottom + normaliseVal), 0.f});
}
