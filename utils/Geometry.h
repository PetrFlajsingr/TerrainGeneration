//
// Created by petr on 10/27/19.
//

#ifndef TERRAINGENERATION_GEOMETRY_H
#define TERRAINGENERATION_GEOMETRY_H

#include <array>
#include <glm/glm.hpp>
#include <iostream>

namespace geo {
struct ViewFrustum {
  std::array<glm::vec4, 6> planes;

  static ViewFrustum FromProjectionView(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);
};

struct AABB {
  glm::vec3 p1, p2;

  friend std::ostream&operator<<(std::ostream&stream, const AABB &aabb);
};

bool isAABBInViewFrustum(const AABB &aabb, const ViewFrustum &viewFrustum);

} // namespace geo

#endif // TERRAINGENERATION_GEOMETRY_H
