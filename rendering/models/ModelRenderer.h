//
// Created by petr on 11/29/19.
//

#ifndef TERRAINGENERATION_MODELRENDERER_H
#define TERRAINGENERATION_MODELRENDERER_H

#include "GraphicsModel.h"
#include "GraphicsModelBase.h"
#include "types/CachedProperty.h"
#include "types/Range.h"
#include <error_handling/exceptions.h>
#include <fplus/fplus.hpp>
#include <geGL/StaticCalls.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <graphics/geGL_utils.h>
#include <memory>
#include <string>
#include <tiny_obj_loader.h>
#include <utility>
#include <vector>

class ObjModelLoader {
public:
  explicit ObjModelLoader(std::string assetsPath);

  template <typename BufferType = ge::gl::Buffer>
  std::shared_ptr<GraphicsModelBase> loadModel(std::string name,
                                               const std::string &id);

private:
  std::string assetsPath;
};


class SceneLoader;
class ModelRenderer {
public:
  GraphicsModelBase &addModel(std::shared_ptr<GraphicsModelBase> model);

  void loadScene(SceneLoader &&sceneLoader);

  std::optional<std::shared_ptr<GraphicsModelBase>>
  modelById(const GraphicsModelBase::Id &id);

  [[nodiscard]] const std::vector<std::shared_ptr<GraphicsModelBase>> &getModels() const;

  void render(const std::shared_ptr<ge::gl::Program> &program, glm::mat4 view,
              bool wa);

  void plainRender();

private:
  std::vector<std::shared_ptr<GraphicsModelBase>> models;
};

template <typename BufferType>
std::shared_ptr<GraphicsModelBase>
ObjModelLoader::loadModel(std::string name, const std::string &id) {
  if (name.substr(name.size() - 4) == ".obj") {
    name = name.substr(0, name.size() - 4);
  }
  using namespace MakeRange;
  const auto path = assetsPath + '/' + name + ".obj";
  auto result = std::make_shared<GraphicsModel<BufferType>>(
      id, GraphicsModelBase::Type::UniformColor);

  tinyobj::attrib_t attribs;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  if (!tinyobj::LoadObj(&attribs, &shapes, &materials, nullptr, nullptr,
                        path.c_str())) {
    throw exc::Exception("Obj file could not be loaded: " + path);
  }
  std::vector<uint> indices;
  std::vector<glm::vec4> vertices;
  std::vector<glm::vec3> normals;
  std::vector<int> faceCountsForNormals;

  for (auto i : range(0, attribs.vertices.size(), 3)) {
    vertices.emplace_back(attribs.vertices[i], attribs.vertices[i + 1],
                          attribs.vertices[i + 2], 1);
  }
  normals.resize(vertices.size(), glm::vec3{0, 0, 0});
  faceCountsForNormals.resize(vertices.size(), 0);
  for (auto index : shapes[0].mesh.indices) {
    indices.emplace_back(index.vertex_index);
    normals[index.vertex_index] +=
        glm::vec3{attribs.normals[index.normal_index * 3],
                  attribs.normals[index.normal_index * 3 + 1],
                  attribs.normals[index.normal_index * 3 + 2]};
    faceCountsForNormals[index.vertex_index]++;
  }
  for (auto i : range(normals.size())) {
    normals[i] = normals[i] / static_cast<float>(faceCountsForNormals[i]);
  }

  result->elementBuffer = createBuffer(indices);
  result->vertexBuffer = createBuffer(vertices);
  result->normalBuffer = createBuffer(normals);
  result->vertexArray = std::make_shared<ge::gl::VertexArray>();
  result->vertexArray->addAttrib(result->vertexBuffer, 0, 4, GL_FLOAT,
                                 sizeof(float) * 4, 0, GL_FALSE);
  result->vertexArray->addAttrib(result->normalBuffer, 1, 3, GL_FLOAT,
                                 sizeof(float) * 3, 0, GL_FALSE);
  result->vertexArray->addElementBuffer(result->elementBuffer);

  return result;
}

#endif // TERRAINGENERATION_MODELRENDERER_H
