//
// Created by Petr on 19.11.2019.
//

#ifndef UTILITIES_BOUNDINGBOX_H
#define UTILITIES_BOUNDINGBOX_H

#include "common.h"
#include <glm/glm.hpp>
#include <ostream>

namespace geo {
template <unsigned int Dimensions = 3> struct BoundingBox {
  using Point = std::conditional_t<Dimensions == 3, glm::vec3, glm::vec2>;
  Point p1, p2;

  BoundingBox() = default;
  BoundingBox(glm::vec3 p1, glm::vec3 p2) : p1(p1), p2(p2) {}

  bool operator==(const BoundingBox &rhs) const;

  bool operator!=(const BoundingBox &rhs) const;

  [[nodiscard]] RelativePosition contains(const BoundingBox<3> &aabb) const;

  [[nodiscard]] bool contains(glm::vec3 point) const;
};
template <unsigned int Dims>
std::ostream &operator<<(std::ostream &stream, const BoundingBox<Dims> &aabb);
} // namespace geo

#include "BoundingBox.tpp"
#endif // UTILITIES_BOUNDINGBOX_H
