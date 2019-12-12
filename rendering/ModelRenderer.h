//
// Created by petr on 11/29/19.
//

#ifndef TERRAINGENERATION_MODELRENDERER_H
#define TERRAINGENERATION_MODELRENDERER_H

#include "types/CachedProperty.h"
#include "types/Range.h"
#include <error_handling/exceptions.h>
#include <fplus/fplus.hpp>
#include <geGL/StaticCalls.h>
#include <geGL/geGL.h>
#include <geGL_utils.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <io/print.h>
#include <string>
#include <tiny_obj_loader.h>
#include <utility>

class ObjModelLoader;

class GraphicsModelBase {
  friend class ObjModelLoader;

public:
  using Id = std::string;
  enum Type { UniformColor, Texture, ProceduralTexture };

  GraphicsModelBase(Id id, Type type) : id(std::move(id)), type(type) {}
  ~GraphicsModelBase() = default;

  [[nodiscard]] const std::shared_ptr<ge::gl::VertexArray> &
  getVertexArray() const;
  [[nodiscard]] const Id &getId() const;
  [[nodiscard]] Type getType() const;

  [[nodiscard]] const glm::vec3 &getPosition() const;
  GraphicsModelBase &setPosition(const glm::vec3 &position);
  GraphicsModelBase &setPosition(float x, float y, float z);
  [[nodiscard]] const glm::vec3 &getRotation() const;
  GraphicsModelBase &setRotation(const glm::vec3 &rotation);
  GraphicsModelBase &setRotation(float x, float y, float z);
  [[nodiscard]] const glm::vec3 &getScale() const;
  GraphicsModelBase &setScale(const glm::vec3 &scale);
  GraphicsModelBase &setScale(float x, float y, float z);
  GraphicsModelBase &setScale(float scale);

  bool isDrawn() const;
  GraphicsModelBase & setDrawn(bool drawn);

  virtual std::shared_ptr<ge::gl::Buffer> getVertexBuffer() = 0;
  virtual std::shared_ptr<ge::gl::Buffer> getNormalBuffer() = 0;
  virtual std::shared_ptr<ge::gl::Buffer> getElementBuffer() = 0;

  mutable CachedProperty<glm::mat4> modelMatrix{
      [this] { return updateModelMatrix; },
      [this] {
        updateModelMatrix = false;
        auto ModelMatrix = glm::mat4();

        ModelMatrix = glm::translate(ModelMatrix, position);
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.x),glm::vec3(1,0,0));
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.y),glm::vec3(0,1,0));
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.z),glm::vec3(0,0,1));
        ModelMatrix = glm::scale(ModelMatrix, scale);

        return ModelMatrix;
      }};

protected:
  std::shared_ptr<ge::gl::VertexArray> vertexArray;

private:
  Id id;
  Type type;
  bool updateModelMatrix = true;

  bool drawn = true;

  glm::vec3 position{0, 0, 0};
  glm::vec3 rotation{0, 0, 0};
  glm::vec3 scale{1, 1, 1};
};

template <typename BufferType = ge::gl::Buffer>
class GraphicsModel : public GraphicsModelBase {
  friend class ObjModelLoader;
  static_assert(std::is_base_of_v<ge::gl::Buffer, BufferType>);

public:
  GraphicsModel(const Id &id, Type type);

  std::shared_ptr<ge::gl::Buffer> getVertexBuffer() override;
  std::shared_ptr<ge::gl::Buffer> getNormalBuffer() override;
  std::shared_ptr<ge::gl::Buffer> getElementBuffer() override;

protected:
  std::shared_ptr<BufferType> vertexBuffer;
  std::shared_ptr<BufferType> normalBuffer;
  std::shared_ptr<BufferType> elementBuffer;
};

template <typename BufferType = ge::gl::Buffer>
class InstancedGraphicsModel : public GraphicsModel<BufferType> {
public:

};

class ObjModelLoader {
public:
  explicit ObjModelLoader(std::string assetsPath);

  template <typename BufferType = ge::gl::Buffer>
  std::shared_ptr<GraphicsModelBase> loadModel(std::string name,
                                               const std::string &id);

private:
  std::string assetsPath;
};

template <typename BufferType>
std::shared_ptr<ge::gl::Buffer> GraphicsModel<BufferType>::getVertexBuffer() {
  return vertexBuffer;
}
template <typename BufferType>
std::shared_ptr<ge::gl::Buffer> GraphicsModel<BufferType>::getNormalBuffer() {
  return normalBuffer;
}
template <typename BufferType>
std::shared_ptr<ge::gl::Buffer> GraphicsModel<BufferType>::getElementBuffer() {
  return elementBuffer;
}
template <typename BufferType>
GraphicsModel<BufferType>::GraphicsModel(const GraphicsModelBase::Id &id,
                                         GraphicsModelBase::Type type)
    : GraphicsModelBase(id, type) {}

class ModelRenderer {
public:
  GraphicsModelBase &addModel(std::shared_ptr<GraphicsModelBase> model);

  std::optional<std::shared_ptr<GraphicsModelBase>>
  modelById(const GraphicsModelBase::Id &id);

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
