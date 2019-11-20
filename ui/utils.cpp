//
// Created by petr on 11/20/19.
//

#include "utils.h"
std::array<glm::vec3, 4> sdlRectToGLCoordinates(const SDL_Rect &rect,
                                                unsigned int screenWidth,
                                                unsigned int screenHeight) {
  constexpr float normaliseVal = -1.f;
  float xLeft = static_cast<float>(rect.x) / screenWidth;
  float xRight = static_cast<float>(rect.x + rect.w) / screenWidth;
  float yTop = static_cast<float>(rect.y) / screenHeight;
  float yBottom = static_cast<float>(rect.y + rect.h) / screenHeight;
  return std::experimental::make_array(
      glm::vec3{xRight + normaliseVal, -(yTop + normaliseVal), 0.f},
      glm::vec3{xLeft + normaliseVal, -(yTop + normaliseVal), 0.f},
      glm::vec3{xRight + normaliseVal, -(yBottom + normaliseVal), 0.f},
      glm::vec3{xLeft + normaliseVal, -(yBottom + normaliseVal), 0.f});
}
