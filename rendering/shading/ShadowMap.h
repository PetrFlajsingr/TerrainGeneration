//
// Created by petr on 12/5/19.
//

#ifndef TERRAINGENERATION_SHADOWMAP_H
#define TERRAINGENERATION_SHADOWMAP_H

#include "utils/geGL_utils.h"
#include <geGL/geGL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace ShaderLiterals;

class ShadowMap {
public:

  ShadowMap(glm::mat4 lightProjection, glm::vec3 lightPos, glm::vec3 target, unsigned int textureWidth, unsigned int textureHeight);

  void begin();
  void end();

  [[nodiscard]] unsigned int getWidth() const;
  [[nodiscard]] unsigned int getHeight() const;
  [[nodiscard]] const ge::gl::Texture &getDepthMap() const;
  [[nodiscard]] const ge::gl::Framebuffer &getDepthMapFbo() const;

  [[nodiscard]] const glm::mat4 &getLightProjection() const;
  void setLightProjection(const glm::mat4 &lightProjection);
  [[nodiscard]] const glm::vec3 &getLightPos() const;
  void setLightPos(const glm::vec3 &lightPos);
  [[nodiscard]] const glm::vec3 &getTarget() const;
  void setTarget(const glm::vec3 &target);

  [[nodiscard]] const std::shared_ptr<ge::gl::Program> &getProgram() const;

  [[nodiscard]] const glm::mat4 &getLightSpaceMatrix() const;

private:
  glm::mat4 lightProjection;
  glm::mat4 lightSpaceMatrix;
  glm::vec3 lightPos;
  glm::vec3 target{0.0f, 0.0f, 0.0f};
  glm::vec3 up{0.0f, 1.0f, 0.0f};
  GLint m_viewport[4]{};

  unsigned int width = 4096;
  unsigned int height = 4096;

  ge::gl::Texture depthMap {GL_TEXTURE_2D, GL_DEPTH_COMPONENT, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height)};

  ge::gl::Framebuffer depthMapFBO;
  std::shared_ptr<ge::gl::Program> program = std::make_shared<ge::gl::Program>(
      "shadow_map/sm"_vert, "shadow_map/sm"_frag);
};

#endif // TERRAINGENERATION_SHADOWMAP_H
