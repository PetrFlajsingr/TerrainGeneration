//
// Created by Petr on 19.11.2019.
//

template <unsigned int Dims>
std::ostream &operator<<(std::ostream &stream,
                         const geo::BoundingBox<Dims> &aabb) {
  stream << "P1: [" << aabb.p1.x << ", " << aabb.p1.y << ", " << aabb.p1.z
         << "], P2: [ "
         << "]" << aabb.p2.x << ", " << aabb.p2.y << ", " << aabb.p2.z << "]";
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
