//
// Created by petr on 12/8/19.
//

#ifndef TERRAINGENERATION_CASCADEDSHADOWMAP_H
#define TERRAINGENERATION_CASCADEDSHADOWMAP_H

#include "glm/gtc//type_ptr.hpp"
#include <Camera.h>
#include <geGL/StaticCalls.h>
#include <geGL/geGL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <graphics/BoundingBox.h>
#include <graphics/TempViewportSetter.h>
#include <graphics/geGL_utils.h>
#include <memory>
#include <meta/meta.h>
#include <utils/types/Range.h>
#include <vector>

class CascadedShadowMap {
public:
  CascadedShadowMap(unsigned int cascadeCount, unsigned int size);

  void bindRender(const std::shared_ptr<ge::gl::Program> &program);

  [[nodiscard]] const glm::vec3 &getLightDir() const;
  void setLightDir(const glm::vec3 &lightDir);

  [[nodiscard]] const std::vector<float> &getCascadeSplits() const;
  [[nodiscard]] GLuint getDepthMap() const;

  [[nodiscard]] unsigned int getCascadeCount() const;

  [[nodiscard]] float getLambda() const;
  void setLambda(float lambda);

  [[nodiscard]] float getMinDistance() const;
  void setMinDistance(float minDistance);

  [[nodiscard]] float getMaxDistance() const;
  void setMaxDistance(float maxDistance);

  template <typename F>
  void renderShadowMap(F renderFunction, const PerspectiveProjection &perspectiveProjection, const glm::mat4 &cameraView);

private:
  std::vector<glm::mat4> lightViewMatrix;
  std::vector<glm::mat4> lightOrthoMatrix;
  std::vector<glm::mat4> cascadedMatrices;
  std::vector<geo::BoundingBox<3>> bbs;
  std::vector<float> cascadeSplitArray;
  glm::vec3 lightDir;

  unsigned int cascadeCount;
  unsigned int size;

  float lambda = .7f;
  float minDistance = 0.0f;
  float maxDistance = 1.0f;

  GLuint depthMap;
  GLuint depthMapFBO;

  std::shared_ptr<ge::gl::Program> program;

  void calculateOrthoMatrices(const glm::mat4 &cameraProjection, const glm::mat4 &cameraView, float cameraNear, float cameraFar,
                              float aspectRatio, float fieldOfView);

  void bindCascade(unsigned int index);
};

template <typename F> void CascadedShadowMap::renderShadowMap(F renderFunction,
                                                              const PerspectiveProjection &perspectiveProjection,
                                                              const glm::mat4 &cameraView) {
  using namespace MakeRange;
  calculateOrthoMatrices(perspectiveProjection.matrix.getRef(), cameraView, perspectiveProjection.getNear(),
                         perspectiveProjection.getFar(), perspectiveProjection.getAspectRatio(),
                         perspectiveProjection.getFieldOfView());

  program->use();
  TempViewportSetter viewportSetter{{0, 0, static_cast<int>(size), static_cast<int>(size)}};
  ge::gl::glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

  for (auto i : range(cascadeCount)) {
    bindCascade(i);
    ge::gl::glClear(GL_DEPTH_BUFFER_BIT);
    ge::gl::glEnable(GL_DEPTH_TEST);
    ge::gl::glEnable(GL_DEPTH_CLAMP);
    ge::gl::glCullFace(GL_BACK);
    const auto lightViewProjection = lightOrthoMatrix[i] * lightViewMatrix[i];
    program->setMatrix4fv("lightViewProjectionMatrix", glm::value_ptr(lightViewProjection));
    renderFunction(program, bbs[i]);
  }
  ge::gl::glDisable(GL_DEPTH_CLAMP);
  ge::gl::glBindFramebuffer(GL_FRAMEBUFFER, 0);
  ge::gl::glCullFace(GL_BACK);
}

#endif // TERRAINGENERATION_CASCADEDSHADOWMAP_H
