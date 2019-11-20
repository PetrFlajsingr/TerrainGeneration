//
// Created by Petr on 19.11.2019.
//

#ifndef UTILITIES_BOUNDINGBOX_H
#define UTILITIES_BOUNDINGBOX_H

#include <glm/detail/type_vec3.hpp>
#include <glm/glm.hpp>
#include <ostream>

namespace geo {
template <unsigned int Dimensions = 3> struct BoundingBox {
  using Point = std::conditional_t<Dimensions == 3, glm::vec3, glm::vec2>;
  Point p1, p2;

  bool operator==(const BoundingBox &rhs) const;

  bool operator!=(const BoundingBox &rhs) const;

  template <unsigned int Dims>
  friend std::ostream &operator<<(std::ostream &stream,
                                  const BoundingBox<Dims> &aabb);
};
} // namespace geo

#include "BoundingBox.tpp"
#endif // UTILITIES_BOUNDINGBOX_H
