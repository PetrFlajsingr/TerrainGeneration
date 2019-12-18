//
// Created by petr on 12/15/19.
//

#ifndef TERRAINGENERATION_GRAPHICSMODEL_H
#define TERRAINGENERATION_GRAPHICSMODEL_H

#include "GraphicsModelBase.h"
#include <memory>

template <typename BufferType = ge::gl::Buffer> class GraphicsModel : public GraphicsModelBase {
  friend class ObjModelLoader;
  static_assert(std::is_base_of_v<ge::gl::Buffer, BufferType>);

public:
  GraphicsModel(const Id &id, Type type);
  GraphicsModel(const GraphicsModel &other);
  GraphicsModel &operator=(const GraphicsModel &other);

  std::shared_ptr<ge::gl::Buffer> getVertexBuffer() override;
  std::shared_ptr<ge::gl::Buffer> getNormalBuffer() override;
  std::shared_ptr<ge::gl::Buffer> getElementBuffer() override;

protected:
  std::shared_ptr<BufferType> vertexBuffer;
  std::shared_ptr<BufferType> normalBuffer;
  std::shared_ptr<BufferType> elementBuffer;
};

template <typename BufferType> std::shared_ptr<ge::gl::Buffer> GraphicsModel<BufferType>::getVertexBuffer() {
  return vertexBuffer;
}
template <typename BufferType> std::shared_ptr<ge::gl::Buffer> GraphicsModel<BufferType>::getNormalBuffer() {
  return normalBuffer;
}
template <typename BufferType> std::shared_ptr<ge::gl::Buffer> GraphicsModel<BufferType>::getElementBuffer() {
  return elementBuffer;
}
template <typename BufferType>
GraphicsModel<BufferType>::GraphicsModel(const GraphicsModelBase::Id &id, GraphicsModelBase::Type type)
    : GraphicsModelBase(id, type) {}
template <typename BufferType> GraphicsModel<BufferType>::GraphicsModel(const GraphicsModel &other) : GraphicsModelBase(other) {
  vertexBuffer = other.vertexBuffer;
  normalBuffer = other.normalBuffer;
  elementBuffer = other.elementBuffer;
  vertexArray = other.vertexArray;
}
template <typename BufferType> GraphicsModel<BufferType> &GraphicsModel<BufferType>::operator=(const GraphicsModel &other) {
  GraphicsModelBase::operator=(other);
  vertexBuffer = other.vertexBuffer;
  normalBuffer = other.normalBuffer;
  elementBuffer = other.elementBuffer;
  vertexArray = other.vertexArray;
  return *this;
}

#endif // TERRAINGENERATION_GRAPHICSMODEL_H
