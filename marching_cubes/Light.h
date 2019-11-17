//
// Created by petr on 11/17/19.
//

#ifndef TERRAINGENERATION_LIGHT_H
#define TERRAINGENERATION_LIGHT_H

#include <glm/vec3.hpp>

struct BlinnPhongLight {
  glm::vec3 color;
  glm::vec3 position;
  float power;
  glm::vec3 ambientColor;
  glm::vec3 diffuseColor;
  glm::vec3 specColor;
};

struct BlinnPhongMaterial {
  float shininess;
  glm::vec3 color;
};

#endif // TERRAINGENERATION_LIGHT_H
