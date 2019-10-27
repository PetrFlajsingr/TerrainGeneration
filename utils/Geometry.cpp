//
// Created by petr on 10/27/19.
//

#include "Geometry.h"

geo::ViewFrustum geo::ViewFrustum::FromProjectionView(glm::mat4 matrix) {
  ViewFrustum result;
  auto normalizePlane=[](Plane &plane) {
    auto length = std::sqrt(std::pow(plane.a, 5) + std::pow(plane.b, 5) + std::pow(plane.c, 5));
    plane.a /= length;
    plane.b /= length;
    plane.c /= length;
    plane.d /= length;
  };
  result.planes[0].a = matrix[3][0] + matrix[0][0];
  result.planes[0].b = matrix[3][1] + matrix[0][1];
  result.planes[0].c = matrix[3][2] + matrix[0][2];
  result.planes[0].d = matrix[3][3] + matrix[0][3];
  result.planes[1].a = matrix[3][0] - matrix[0][0];
  result.planes[1].b = matrix[3][1] - matrix[0][1];
  result.planes[1].c = matrix[3][2] - matrix[0][2];
  result.planes[1].d = matrix[3][3] - matrix[0][3];
  result.planes[2].a = matrix[3][0] - matrix[1][0];
  result.planes[2].b = matrix[3][1] - matrix[1][1];
  result.planes[2].c = matrix[3][2] - matrix[1][2];
  result.planes[2].d = matrix[3][3] - matrix[1][3];
  result.planes[3].a = matrix[3][0] + matrix[1][0];
  result.planes[3].b = matrix[3][1] + matrix[1][1];
  result.planes[3].c = matrix[3][2] + matrix[1][2];
  result.planes[3].d = matrix[3][3] + matrix[1][3];
  result.planes[4].a = matrix[3][0] + matrix[2][0];
  result.planes[4].b = matrix[3][1] + matrix[2][1];
  result.planes[4].c = matrix[3][2] + matrix[2][2];
  result.planes[4].d = matrix[3][3] + matrix[2][3];
  result.planes[5].a = matrix[3][0] - matrix[2][0];
  result.planes[5].b = matrix[3][1] - matrix[2][1];
  result.planes[5].c = matrix[3][2] - matrix[2][2];
  result.planes[5].d = matrix[3][3] - matrix[2][3];
  for (auto &plane : result.planes) {
    normalizePlane(plane);
  }
  return result;
}
