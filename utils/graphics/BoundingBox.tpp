//
// Created by Petr on 19.11.2019.
//

#include "BoundingBox.h"

template <unsigned int Dims>
std::ostream &geo::operator<<(std::ostream &stream,
                         const geo::BoundingBox<Dims> &aabb) {
  stream << "P1: [" << aabb.p1.x << ", " << aabb.p1.y << ", " << aabb.p1.z
         << "], P2: [ "
         << aabb.p2.x << ", " << aabb.p2.y << ", " << aabb.p2.z << "]";
  return stream;
}

template <unsigned int Dimensions>
bool geo::BoundingBox<Dimensions>::operator==(
    const geo::BoundingBox<Dimensions> &rhs) const {
  return p1 == rhs.p1 && p2 == rhs.p2;
}

template <unsigned int Dimensions>
bool geo::BoundingBox<Dimensions>::operator!=(
    const geo::BoundingBox<Dimensions> &rhs) const {
  return !(rhs == *this);
}
template <unsigned int Dimensions>
geo::RelativePosition
geo::BoundingBox<Dimensions>::contains(const geo::BoundingBox<3> &aabb) const {
  const bool minIn = p1.x < aabb.p1.x && p1.y < aabb.p1.y && p1.z < aabb.p1.z
      && p2.x > aabb.p1.x && p2.y > aabb.p1.y && p2.z > aabb.p1.z;
  const bool maxIn = p2.x > aabb.p2.x && p2.y > aabb.p2.y && p2.z > aabb.p2.z
      && p1.x < aabb.p2.x && p1.y < aabb.p2.y && p1.z < aabb.p2.z;
  if (minIn && maxIn) {
    return RelativePosition::Inside;
  }
  if (minIn || maxIn) {
    return RelativePosition::Intersection;
  }
  return RelativePosition::Outside;
}
