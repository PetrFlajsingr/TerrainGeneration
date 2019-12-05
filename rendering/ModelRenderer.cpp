//
// Created by petr on 11/29/19.
//

#include "ModelRenderer.h"

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
GraphicsModelBase & GraphicsModelBase::setDrawn(bool drawn) {
  GraphicsModelBase::drawn = drawn;
  return *this;
}

ObjModelLoader::ObjModelLoader(std::string assetsPath)
    : assetsPath(std::move(assetsPath)) {}

GraphicsModelBase &
ModelRenderer::addModel(std::shared_ptr<GraphicsModelBase> model) {
  models.emplace_back(std::move(model));
  return *models.back().get();
}
void ModelRenderer::render(const std::shared_ptr<ge::gl::Program> &program,
                           glm::mat4 view, bool wa) {
  for (auto &model : models) {
    if (!model->isDrawn()) {
      continue;
    }
    if (wa) {
      program->setMatrix4fv("model", &model->modelMatrix.getRef()[0][0]);
      program->setMatrix4fv("view", &view[0][0]);
    } else {
      program->setMatrix4fv("model", &model->modelMatrix.getRef()[0][0]);
    }
    model->getVertexArray()->bind();
    ge::gl::glDrawElements(GL_TRIANGLES, model->getElementBuffer()->getSize(),
                           GL_UNSIGNED_INT, nullptr);
  }
}
std::optional<std::shared_ptr<GraphicsModelBase>>
ModelRenderer::modelById(const GraphicsModelBase::Id &id) {
  if (auto iter = fplus::find_first_by(
          [id](const auto &model) { return model->getId() == id; }, models);
      iter.is_just()) {
    return iter.unsafe_get_just();
  }
  return std::nullopt;
}
void ModelRenderer::plainRender() {
  for (auto &model : models) {
    if (!model->isDrawn()) {
      continue;
    }
    model->getVertexArray()->bind();
    ge::gl::glDrawElements(GL_TRIANGLES, model->getElementBuffer()->getSize(),
                           GL_UNSIGNED_INT, nullptr);
  }
}
