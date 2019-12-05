//
// Created by petr on 12/5/19.
//

#include "ShadowMap.h"
#include <geGL/StaticCalls.h>


ShadowMap::ShadowMap(glm::mat4 lightProjection, glm::vec3 lightPos, glm::vec3 target, unsigned int textureWidth, unsigned int textureHeight)
: lightProjection(lightProjection), lightPos(lightPos), target(target), width(textureWidth), height(textureHeight) {
  ge::gl::glTextureImage2DEXT(depthMap.getId(), GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0,
                              GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
  depthMap.texParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  depthMap.texParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  depthMap.texParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  depthMap.texParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glm::vec4 borderColor{1.0, 1.0, 1.0, 1.0};
  depthMap.texParameterfv(GL_TEXTURE_BORDER_COLOR, &borderColor[0]);

  depthMapFBO.attachTexture(GL_DEPTH_ATTACHMENT, &depthMap);

  ge::gl::glFramebufferDrawBufferEXT(depthMapFBO.getId(), GL_NONE);
  ge::gl::glFramebufferReadBufferEXT(depthMapFBO.getId(), GL_NONE);
}
void ShadowMap::begin() {
  ge::gl::glGetIntegerv(GL_VIEWPORT, m_viewport);

  program->use();
  ge::gl::glViewport(0, 0, width, height);
  depthMapFBO.bind(GL_FRAMEBUFFER);
  ge::gl::glClear(GL_DEPTH_BUFFER_BIT);

  glm::mat4 lightView = glm::lookAt(lightPos, target, up);

  lightSpaceMatrix = lightProjection * lightView;
  program->setMatrix4fv("lightSpaceMatrix", &lightSpaceMatrix[0][0]);
}
void ShadowMap::end() {
  depthMapFBO.unbind(GL_FRAMEBUFFER);
  auto [x, y, width, height] = m_viewport;
  ge::gl::glViewport(x, y, width, height);
}
unsigned int ShadowMap::getWidth() const { return width; }
unsigned int ShadowMap::getHeight() const { return height; }
const ge::gl::Texture &ShadowMap::getDepthMap() const { return depthMap; }
const ge::gl::Framebuffer &ShadowMap::getDepthMapFbo() const {
  return depthMapFBO;
}
const glm::mat4 &ShadowMap::getLightProjection() const {
  return lightProjection;
}
void ShadowMap::setLightProjection(const glm::mat4 &lightProjection) {
  ShadowMap::lightProjection = lightProjection;
}
const glm::vec3 &ShadowMap::getLightPos() const { return lightPos; }
void ShadowMap::setLightPos(const glm::vec3 &lightPos) {
  ShadowMap::lightPos = lightPos;
}
const glm::vec3 &ShadowMap::getTarget() const { return target; }
void ShadowMap::setTarget(const glm::vec3 &target) {
  ShadowMap::target = target;
}
const std::shared_ptr<ge::gl::Program> &ShadowMap::getProgram() const {
  return program;
}
const glm::mat4 &ShadowMap::getLightSpaceMatrix() const {
  return lightSpaceMatrix;
}
