//
// Created by petr on 10/27/19.
//

#include "Geometry.h"
#include <iostream>

geo::ViewFrustum
geo::ViewFrustum::FromProjectionView(const glm::mat4 &viewMatrix,
                                     const glm::mat4 &projectionMatrix) {
  ViewFrustum result;
  const auto &v = viewMatrix;
  const auto &p = projectionMatrix;

  glm::mat4 clipMatrix;

  clipMatrix[0][0] = v[0][0] * p[0][0] + v[0][1] * p[1][0] + v[0][2] * p[2][0] +
                     v[0][3] * p[3][0];
  clipMatrix[1][0] = v[0][0] * p[0][1] + v[0][1] * p[1][1] + v[0][2] * p[2][1] +
                     v[0][3] * p[3][1];
  clipMatrix[2][0] = v[0][0] * p[0][2] + v[0][1] * p[1][2] + v[0][2] * p[2][2] +
                     v[0][3] * p[3][2];
  clipMatrix[3][0] = v[0][0] * p[0][3] + v[0][1] * p[1][3] + v[0][2] * p[2][3] +
                     v[0][3] * p[3][3];
  clipMatrix[0][1] = v[1][0] * p[0][0] + v[1][1] * p[1][0] + v[1][2] * p[2][0] +
                     v[1][3] * p[3][0];
  clipMatrix[1][1] = v[1][0] * p[0][1] + v[1][1] * p[1][1] + v[1][2] * p[2][1] +
                     v[1][3] * p[3][1];
  clipMatrix[2][1] = v[1][0] * p[0][2] + v[1][1] * p[1][2] + v[1][2] * p[2][2] +
                     v[1][3] * p[3][2];
  clipMatrix[3][1] = v[1][0] * p[0][3] + v[1][1] * p[1][3] + v[1][2] * p[2][3] +
                     v[1][3] * p[3][3];
  clipMatrix[0][2] = v[2][0] * p[0][0] + v[2][1] * p[1][0] + v[2][2] * p[2][0] +
                     v[2][3] * p[3][0];
  clipMatrix[1][2] = v[2][0] * p[0][1] + v[2][1] * p[1][1] + v[2][2] * p[2][1] +
                     v[2][3] * p[3][1];
  clipMatrix[2][2] = v[2][0] * p[0][2] + v[2][1] * p[1][2] + v[2][2] * p[2][2] +
                     v[2][3] * p[3][2];
  clipMatrix[3][2] = v[2][0] * p[0][3] + v[2][1] * p[1][3] + v[2][2] * p[2][3] +
                     v[2][3] * p[3][3];
  clipMatrix[0][3] = v[3][0] * p[0][0] + v[3][1] * p[1][0] + v[3][2] * p[2][0] +
                     v[3][3] * p[3][0];
  clipMatrix[1][3] = v[3][0] * p[0][1] + v[3][1] * p[1][1] + v[3][2] * p[2][1] +
                     v[3][3] * p[3][1];
  clipMatrix[2][3] = v[3][0] * p[0][2] + v[3][1] * p[1][2] + v[3][2] * p[2][2] +
                     v[3][3] * p[3][2];
  clipMatrix[3][3] = v[3][0] * p[0][3] + v[3][1] * p[1][3] + v[3][2] * p[2][3] +
                     v[3][3] * p[3][3];

  result.planes[0].x = clipMatrix[3][0] - clipMatrix[0][0];
  result.planes[0].y = clipMatrix[3][1] - clipMatrix[0][1];
  result.planes[0].z = clipMatrix[3][2] - clipMatrix[0][2];
  result.planes[0].w = clipMatrix[3][3] - clipMatrix[0][3];

  result.planes[1].x = clipMatrix[3][0] + clipMatrix[0][0];
  result.planes[1].y = clipMatrix[3][1] + clipMatrix[0][1];
  result.planes[1].z = clipMatrix[3][2] + clipMatrix[0][2];
  result.planes[1].w = clipMatrix[3][3] + clipMatrix[0][3];

  result.planes[2].x = clipMatrix[3][0] + clipMatrix[1][0];
  result.planes[2].y = clipMatrix[3][1] + clipMatrix[1][1];
  result.planes[2].z = clipMatrix[3][2] + clipMatrix[1][2];
  result.planes[2].w = clipMatrix[3][3] + clipMatrix[1][3];

  result.planes[3].x = clipMatrix[3][0] - clipMatrix[1][0];
  result.planes[3].y = clipMatrix[3][1] - clipMatrix[1][1];
  result.planes[3].z = clipMatrix[3][2] - clipMatrix[1][2];
  result.planes[3].w = clipMatrix[3][3] - clipMatrix[1][3];

  result.planes[4].x = clipMatrix[3][0] - clipMatrix[2][0];
  result.planes[4].y = clipMatrix[3][1] - clipMatrix[2][1];
  result.planes[4].z = clipMatrix[3][2] - clipMatrix[2][2];
  result.planes[4].w = clipMatrix[3][3] - clipMatrix[2][3];

  result.planes[5].x = clipMatrix[3][0] + clipMatrix[2][0];
  result.planes[5].y = clipMatrix[3][1] + clipMatrix[2][1];
  result.planes[5].z = clipMatrix[3][2] + clipMatrix[2][2];
  result.planes[5].w = clipMatrix[3][3] + clipMatrix[2][3];

  for (int i = 0; i < 6; i++) {
    result.planes[i] = glm::normalize(result.planes[i]);
  }
  return result;
}

bool geo::isAABBInViewFrustum(const geo::AABB &aabb,
                              const geo::ViewFrustum &viewFrustum) {
  static auto createPoint = [](auto &first, auto &second, auto &normal) {
    glm::vec3 result = first;

    if (normal.x >= 0.0f)
      result.x = second.x;
    if (normal.y >= 0.0f)
      result.y = second.y;
    if (normal.z >= 0.0f)
      result.z = second.z;

    return result;
  };
  for (auto i = 0; i < 6; i++) {
    const float pos = viewFrustum.planes[i].w;
    const auto normal = glm::vec3(viewFrustum.planes[i]);

    if (glm::dot(normal, createPoint(aabb.p1, aabb.p2, normal)) + pos < 0.0f) {
      return false;
    }

    if (glm::dot(normal, createPoint(aabb.p2, aabb.p1, normal)) + pos < 0.0f) {
      return true;
    }
  }
  return true;
}
std::ostream &geo::operator<<(std::ostream &stream, const geo::AABB &aabb) {
  stream << "P1: [" << aabb.p1.x << ", " << aabb.p1.y << ", " << aabb.p1.z
         << "], P2: [ "
         << "]" << aabb.p2.x << ", " << aabb.p2.y << ", " << aabb.p2.z << "]";
  return stream;
}
bool geo::AABB::operator==(const geo::AABB &rhs) const {
  return p1 == rhs.p1 && p2 == rhs.p2;
}
bool geo::AABB::operator!=(const geo::AABB &rhs) const {
  return !(rhs == *this);
}
bool geo::isBoundingSphereInViewFrustum(const geo::BoundingSphere &bs,
                                        const geo::ViewFrustum &viewFrustum) {
  auto center = glm::vec4{bs.center, 1.0};
  auto dist01 = std::min(distanceToPlane(viewFrustum.planes[0], center),
                         distanceToPlane(viewFrustum.planes[1], center));
  if (dist01 <= 0)
    return false;
  auto dist23 = std::min(distanceToPlane(viewFrustum.planes[2], center),
                         distanceToPlane(viewFrustum.planes[3], center));
  if (dist23 <= 0)
    return false;
  auto dist45 = std::min(distanceToPlane(viewFrustum.planes[4], center),
                         distanceToPlane(viewFrustum.planes[5], center));
  return dist45 > 0;
}
glm::vec3 geo::midPoint(const glm::vec3 &p1, const glm::vec3 &p2) {
  return glm::vec3{(p1.x + p2.x) / 2, (p1.y + p2.y) / 2, (p1.z + p2.z) / 2};
}
float geo::distanceToPlane(const glm::vec4 &plane, const glm::vec4 &point) {
  return glm::dot(point, plane);
}
