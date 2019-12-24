//
// Created by petr on 11/29/19.
//
#define TINYOBJLOADER_IMPLEMENTATION

#include "ModelRenderer.h"
#include "SceneLoader.h"

ObjModelLoader::ObjModelLoader(std::string assetsPath) : assetsPath(std::move(assetsPath)) {}

GraphicsModelBase &ModelRenderer::addModel(std::shared_ptr<GraphicsModelBase> model) {
  models.emplace_back(std::move(model));
  return *models.back().get();
}
void ModelRenderer::render(const std::shared_ptr<ge::gl::Program> &program, glm::mat4 view, bool wa) {
  for (auto &model : models) {
    if (!model->isDrawn()) {
      continue;
    }
    const auto modelMatrix = model->modelMatrix.getRef();
    if (wa) {
      program->setMatrix4fv("model", &modelMatrix[0][0]);
      program->setMatrix4fv("view", &view[0][0]);
    } else {
      program->setMatrix4fv("model", &modelMatrix[0][0]);
    }
    model->getVertexArray()->bind();
    ge::gl::glDrawElements(GL_TRIANGLES, model->getElementBuffer()->getSize(), GL_UNSIGNED_INT, nullptr);
  }
}
std::optional<std::shared_ptr<GraphicsModelBase>> ModelRenderer::modelById(const GraphicsModelBase::Id &id) {
  if (auto iter = std::find_if(models.begin(), models.end(), [id](const auto &model) { return model->getId() == id; });
      iter != models.end()) {
    return *iter;
  }
  return std::nullopt;
}
void ModelRenderer::plainRender() {
  for (auto &model : models) {
    if (!model->isDrawn()) {
      continue;
    }
    model->getVertexArray()->bind();
    ge::gl::glDrawElements(GL_TRIANGLES, model->getElementBuffer()->getSize(), GL_UNSIGNED_INT, nullptr);
  }
}
void ModelRenderer::loadScene(SceneLoader &&sceneLoader) {
  for (const auto &models : sceneLoader) {
    for (auto &model : models) {
      addModel(model);
    }
  }
}
const std::vector<std::shared_ptr<GraphicsModelBase>> &ModelRenderer::getModels() const { return models; }
