//
// Created by petr on 1/2/20.
//

#ifndef TERRAINGENERATION_GEGL_H
#define TERRAINGENERATION_GEGL_H
#include "meta/meta.h"
#include <geGL/Buffer.h>
#include <type_traits>

template <typename T> concept geGLBuffer = std::is_base_of_v<ge::gl::Buffer, T>;

template <typename T> concept geGLBufferShared = is_specialization_v<T, std::shared_ptr> &&geGLBuffer<typename T::value_type>;

#endif // TERRAINGENERATION_GEGL_H
