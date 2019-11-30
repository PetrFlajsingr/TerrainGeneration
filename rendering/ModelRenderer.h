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
  [[nodiscard]] const glm::vec3 &getRotation() const;
  GraphicsModelBase & setRotation(const glm::vec3 &rotation);
  [[nodiscard]] const glm::vec3 &getScale() const;
  GraphicsModelBase & setScale(const glm::vec3 &scale);

  virtual std::shared_ptr<ge::gl::Buffer> getVertexBuffer() = 0;
  virtual std::shared_ptr<ge::gl::Buffer> getNormalBuffer() = 0;
  virtual std::shared_ptr<ge::gl::Buffer> getElementBuffer() = 0;

  CachedProperty<glm::mat4> modelMatrix{
      [this] { return updateModelMatrix; },
      [this] {
        updateModelMatrix = false;
        return glm::scale(glm::translate(glm::mat4{}, position), scale);
      }};

protected:
  std::shared_ptr<ge::gl::VertexArray> vertexArray;

private:
  Id id;
  Type type;
  bool updateModelMatrix = true;

  glm::vec3 position;
  glm::vec3 rotation;
  glm::vec3 scale;
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

class ObjModelLoader {
public:
  explicit ObjModelLoader(std::string assetsPath);

  template <typename BufferType = ge::gl::Buffer>
  std::shared_ptr<GraphicsModelBase> loadModel(const std::string &name,
                                               const std::string &id) {
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
    for (auto index : shapes[0].mesh.indices) {
      indices.emplace_back(index.vertex_index);
      normals.emplace_back(attribs.normals[index.normal_index]);
    }

    for (auto i : range(0, attribs.vertices.size(), 3)) {
      vertices.emplace_back(attribs.vertices[i], attribs.vertices[i + 1],
                            attribs.vertices[i + 2], 1);
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
  void addModel(std::shared_ptr<GraphicsModelBase> model);

  std::optional<std::shared_ptr<GraphicsModelBase>>
  modelById(const GraphicsModelBase::Id &id);

  void render(const std::shared_ptr<ge::gl::Program> &program, glm::mat4 view, bool wa);

private:
  std::vector<std::shared_ptr<GraphicsModelBase>> models;
};

#endif // TERRAINGENERATION_MODELRENDERER_H
