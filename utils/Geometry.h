//
// Created by petr on 10/27/19.
//

#ifndef TERRAINGENERATION_GEOMETRY_H
#define TERRAINGENERATION_GEOMETRY_H

#include <array>
#include <glm/glm.hpp>
#include <iostream>

namespace geo {

enum class FrustumPosition {
  Inside, Outside, Intersection
};

struct AABB {
  glm::vec3 p1, p2;

  bool operator==(const AABB &rhs) const;
  bool operator!=(const AABB &rhs) const;

  friend std::ostream&operator<<(std::ostream&stream, const AABB &aabb);
};

struct BoundingSphere {
  glm::vec3 center;
  float radius;
};

struct ViewFrustum {
  std::array<glm::vec4, 6> planes;

  static ViewFrustum FromProjectionView(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);

  [[nodiscard]] FrustumPosition contains(const AABB &aabb) const;
  [[nodiscard]] FrustumPosition contains(const BoundingSphere &bs) const;
};

float distanceToPlane(const glm::vec4 &plane, const glm::vec4 &point);

bool isAABBInViewFrustum(const AABB &aabb, const ViewFrustum &viewFrustum);
bool isBoundingSphereInViewFrustum(const BoundingSphere &bs, const ViewFrustum &viewFrustum);

glm::vec3 midPoint(const glm::vec3 &p1, const glm::vec3 &p2);

} // namespace geo

#endif // TERRAINGENERATION_GEOMETRY_H
