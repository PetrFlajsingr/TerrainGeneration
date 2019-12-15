//
// Created by petr on 12/15/19.
//

#include "GraphicsModelBase.h"

GraphicsModelBase::GraphicsModelBase(GraphicsModelBase::Id id,
                                     GraphicsModelBase::Type type)
    : id(std::move(id)), type(type) {}


const std::shared_ptr<ge::gl::VertexArray> &
GraphicsModelBase::getVertexArray() const {
  return vertexArray;
}
const GraphicsModelBase::Id &GraphicsModelBase::getId() const { return id; }
GraphicsModelBase::Type GraphicsModelBase::getType() const { return type; }

const glm::vec3 &GraphicsModelBase::getPosition() const { return position; }
GraphicsModelBase &GraphicsModelBase::setPosition(const glm::vec3 &position) {
  GraphicsModelBase::position = position;
  updateModelMatrix = true;
  return *this;
}
const glm::vec3 &GraphicsModelBase::getRotation() const { return rotation; }
GraphicsModelBase &GraphicsModelBase::setRotation(const glm::vec3 &rotation) {
  GraphicsModelBase::rotation = rotation;
  updateModelMatrix = true;
  return *this;
}
const glm::vec3 &GraphicsModelBase::getScale() const { return scale; }
GraphicsModelBase &GraphicsModelBase::setScale(const glm::vec3 &scale) {
  GraphicsModelBase::scale = scale;
  updateModelMatrix = true;
  return *this;
}
GraphicsModelBase &GraphicsModelBase::setPosition(float x, float y, float z) {
  setPosition({x, y, z});
  return *this;
}
GraphicsModelBase &GraphicsModelBase::setRotation(float x, float y, float z) {
  setRotation({x, y, z});
  return *this;
}

GraphicsModelBase &GraphicsModelBase::setScale(float x, float y, float z) {
  setScale({x, y, z});
  return *this;
}
bool GraphicsModelBase::isDrawn() const { return drawn; }
GraphicsModelBase &GraphicsModelBase::setDrawn(bool drawn) {
  GraphicsModelBase::drawn = drawn;
  return *this;
}
GraphicsModelBase &GraphicsModelBase::setScale(float scale) {
  setScale(scale, scale, scale);
  return *this;
}
GraphicsModelBase::GraphicsModelBase(const GraphicsModelBase &other) {
  id = other.id;
  type = other.type;
  scale = other.scale;
  position = other.position;
  rotation = other.rotation;
  updateModelMatrix = true;
}
GraphicsModelBase &
GraphicsModelBase::operator=(const GraphicsModelBase &other) {
  id = other.id;
  type = other.type;
  scale = other.scale;
  position = other.position;
  rotation = other.rotation;
  updateModelMatrix = true;
  return *this;
}
void GraphicsModelBase::setId(const GraphicsModelBase::Id &id) {
  GraphicsModelBase::id = id;
}
