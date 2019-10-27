//
// Created by petr on 10/27/19.
//

#ifndef TERRAINGENERATION_GEOMETRY_H
#define TERRAINGENERATION_GEOMETRY_H

#include <array>
#include <glm/glm.hpp>

namespace geo {
struct Plane {
  float a, b, c, d;
};

struct ViewFrustum {
  std::array<Plane, 6> planes;

  static ViewFrustum FromProjectionView(glm::mat4 matrix);
};

struct AABB {
  glm::vec3 p1, p2;
};

bool isAABBInViewFrustum(const AABB &aabb, const ViewFrustum &viewFrustum) {
  for (int planeID = 0; planeID < 6; ++planeID) {
    glm::vec3 planeNormal{viewFrustum.planes[planeID].a,
                          viewFrustum.planes[planeID].b,
                          viewFrustum.planes[planeID].c};
    float planeConstant = viewFrustum.planes[planeID].d;

    glm::vec3 axisVert;

    if (viewFrustum.planes[planeID].a < 0.0f) {
      axisVert.x = aabb.p1.x;
    } else {
      axisVert.x = aabb.p2.x;
    }
    if (viewFrustum.planes[planeID].b < 0.0f) {
      axisVert.y = aabb.p1.y;
    } else {
      axisVert.y = aabb.p2.y;
    }
    if (viewFrustum.planes[planeID].c < 0.0f) {
      axisVert.z = aabb.p1.z;
    } else {
      axisVert.z = aabb.p2.z;
    }

    if (glm::dot(planeNormal, axisVert) + planeConstant < 0.0f) {
      return false;
    }
  }
  return true;
}

} // namespace geo

#endif // TERRAINGENERATION_GEOMETRY_H
