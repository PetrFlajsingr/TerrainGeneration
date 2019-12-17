//
// Created by petr on 12/15/19.
//

#ifndef TERRAINGENERATION_GRAPHICSMODELBASE_H
#define TERRAINGENERATION_GRAPHICSMODELBASE_H

#include "utils/types/CachedProperty.h"
#include <geGL/geGL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

class GraphicsModelBase {
  friend class ObjModelLoader;

public:
  using Id = std::string;
  enum Type { UniformColor, Texture, ProceduralTexture };

  GraphicsModelBase(Id id, Type type);
  virtual ~GraphicsModelBase() = default;
  GraphicsModelBase(const GraphicsModelBase &other);
  GraphicsModelBase &operator=(const GraphicsModelBase &other);

  [[nodiscard]] const std::shared_ptr<ge::gl::VertexArray> &
  getVertexArray() const;
  void setId(const Id &id);
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
  GraphicsModelBase &setDrawn(bool drawn);

  virtual std::shared_ptr<ge::gl::Buffer> getVertexBuffer() = 0;
  virtual std::shared_ptr<ge::gl::Buffer> getNormalBuffer() = 0;
  virtual std::shared_ptr<ge::gl::Buffer> getElementBuffer() = 0;

  mutable CachedProperty<glm::mat4> modelMatrix{
      [this] { return updateModelMatrix; },
      [this] {
        updateModelMatrix = false;
        auto ModelMatrix = glm::mat4();

        ModelMatrix = glm::translate(ModelMatrix, position);
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.x),
                                  glm::vec3(1, 0, 0));
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.y),
                                  glm::vec3(0, 1, 0));
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.z),
                                  glm::vec3(0, 0, 1));
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

#endif // TERRAINGENERATION_GRAPHICSMODELBASE_H
