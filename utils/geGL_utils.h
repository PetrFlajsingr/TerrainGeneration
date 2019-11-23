//
// Created by petr on 10/30/19.
//

#ifndef TERRAINGENERATION_GEGL_UTILS_H
#define TERRAINGENERATION_GEGL_UTILS_H

#include "SparseBuffer.h"
#include "shader_literals.h"
#include <geGL/geGL.h>

template <typename T = uint8_t>
auto createBuffer(std::size_t componentCount, GLbitfield flags = GL_STATIC_DRAW,
                  GLvoid const *data = nullptr) {
  return std::make_shared<ge::gl::Buffer>(componentCount * sizeof(T), data, flags);
}

template <typename Container>
auto createBuffer(Container &&container, GLbitfield flags = GL_STATIC_DRAW) {
  using value_type = typename std::decay_t<Container>::value_type;
  return std::make_shared<ge::gl::Buffer>(container.size() * sizeof(value_type), container.data(), flags);
}

template <typename T = uint8_t>
auto createSparseBuffer(std::size_t componentCount,
                        GLvoid const *data = nullptr) {
  return std::make_shared<SparseBuffer>(componentCount * sizeof(T), data);
}

template <typename Container> auto createSparseBuffer(Container &&container) {
  using value_type = typename std::decay_t<Container>::value_type;
  return std::make_shared<SparseBuffer>(container.size() * sizeof(value_type),
                                        container.data());
}
#endif // TERRAINGENERATION_GEGL_UTILS_H
