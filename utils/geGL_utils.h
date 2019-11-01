//
// Created by petr on 10/30/19.
//

#ifndef TERRAINGENERATION_GEGL_UTILS_H
#define TERRAINGENERATION_GEGL_UTILS_H

#include "shader_literals.h"
#include <geGL/geGL.h>

template <typename T = uint8_t>
std::shared_ptr<ge::gl::Buffer> createBuffer(std::size_t componentCount, GLbitfield flags = GL_STATIC_DRAW, GLvoid const* data  = nullptr) {
  return std::make_shared<ge::gl::Buffer>(componentCount * sizeof(T), data, flags);
}

template <typename Container>
std::shared_ptr<ge::gl::Buffer> createBuffer(Container &&container, GLbitfield flags = GL_STATIC_DRAW) {
  using value_type = typename std::decay_t<Container>::value_type;
  return std::make_shared<ge::gl::Buffer>(container.size() * sizeof(value_type), container.data(), flags);
}

#endif // TERRAINGENERATION_GEGL_UTILS_H
