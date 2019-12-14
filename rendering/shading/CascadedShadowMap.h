//
// Created by petr on 12/8/19.
//

#ifndef TERRAINGENERATION_CASCADEDSHADOWMAP_H
#define TERRAINGENERATION_CASCADEDSHADOWMAP_H
#include "glm/gtc//type_ptr.hpp"
#include <geGL/StaticCalls.h>
#include <geGL/geGL.h>
#include <geGL_utils.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <meta/meta.h>
#include <utils/types/Range.h>

using namespace ShaderLiterals;
class CascadedShadowMap {
public:
  CascadedShadowMap(unsigned int cascadeCount, unsigned int size);

  void bindRender(const std::shared_ptr<ge::gl::Program> &program);

  [[nodiscard]] const glm::vec3 &getLightDir() const;
  void setLightDir(const glm::vec3 &lightDir);

  [[nodiscard]] const std::vector<float> &getCascadeSplits() const;
  [[nodiscard]] GLuint getDepthMap() const;

  [[nodiscard]] unsigned int getCascadeCount() const;

  template <typename F>
  void renderShadowMap(F renderFunction, const glm::mat4 &cameraProjection,
                       const glm::mat4 &cameraView, float cameraNear,
                       float cameraFar, float aspectRatio, float fieldOfView);

private:
  std::vector<glm::mat4> lightViewMatrix;
  std::vector<glm::mat4> lightOrthoMatrix;
  std::vector<glm::mat4> cascadedMatrices;
  std::vector<float> cascadeSplitArray;
  glm::vec3 lightDir;

  unsigned int size = 4096;
  unsigned int cascadeCount;

  GLuint depthMap;

  GLuint depthMapFBO;

  std::shared_ptr<ge::gl::Program> program = std::make_shared<ge::gl::Program>(
      "shadow_map/sm"_vert, "shadow_map/sm"_frag);

  void calculateOrthoMatrices(const glm::mat4 &cameraProjection,
                              const glm::mat4 &cameraView, float cameraNear,
                              float cameraFar, float aspectRatio,
                              float fieldOfView);

  void bindCascade(unsigned int index);

  GLint m_viewport[4]{};
};

template <typename F>
void CascadedShadowMap::renderShadowMap(F renderFunction,
                                        const glm::mat4 &cameraProjection,
                                        const glm::mat4 &cameraView,
                                        float cameraNear, float cameraFar,
                                        float aspectRatio, float fieldOfView) {
  using namespace MakeRange;
  calculateOrthoMatrices(cameraProjection, cameraView, cameraNear, cameraFar,
                         aspectRatio, fieldOfView);
  ge::gl::glGetIntegerv(GL_VIEWPORT, m_viewport);

  program->use();
  ge::gl::glViewport(0, 0, size, size);
  ge::gl::glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

  for (auto i : range(cascadeCount)) {
    bindCascade(i);
    ge::gl::glClear(GL_DEPTH_BUFFER_BIT);
    ge::gl::glEnable(GL_DEPTH_TEST);
    ge::gl::glEnable(GL_DEPTH_CLAMP);
    ge::gl::glCullFace(GL_FRONT);
    const auto lightViewProjection = lightOrthoMatrix[i] * lightViewMatrix[i];
    program->setMatrix4fv("lightViewProjectionMatrix",
                          glm::value_ptr(lightViewProjection));
    renderFunction(program);
  }
  ge::gl::glDisable(GL_DEPTH_CLAMP);
  ge::gl::glBindFramebuffer(GL_FRAMEBUFFER, 0);
  auto [x, y, width, height] = m_viewport;
  ge::gl::glViewport(x, y, width, height);
}

#endif // TERRAINGENERATION_CASCADEDSHADOWMAP_H
