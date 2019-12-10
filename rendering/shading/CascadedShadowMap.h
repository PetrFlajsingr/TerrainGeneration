//
// Created by petr on 12/8/19.
//

#ifndef TERRAINGENERATION_CASCADEDSHADOWMAP_H
#define TERRAINGENERATION_CASCADEDSHADOWMAP_H
#include <geGL/geGL.h>
#include <geGL/StaticCalls.h>
#include <geGL_utils.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <meta/meta.h>
#include <utils/types/Range.h>

using namespace ShaderLiterals;
class CascadedShadowMap {
public:
  CascadedShadowMap(unsigned int cascadeCount, unsigned int textureWidth,
                    unsigned int textureHeight);

  void bindRender();

  [[nodiscard]] const glm::vec3 &getLightPos() const;
  void setLightPos(const glm::vec3 &lightPos);
  [[nodiscard]] const glm::vec3 &getTarget() const;
  void setTarget(const glm::vec3 &target);

  const std::vector<std::unique_ptr<ge::gl::Texture>> &getDepthMaps() const;

  [[nodiscard]] glm::mat4 lightSpaceMatrix(unsigned int cascade) const;

  template <typename F>
  void renderShadowMap(F renderFunction, const glm::mat4 &cameraView,
                       float cameraNear, float cameraFar, float aspectRatio,
                       float fieldOfView);

private:
  static constexpr unsigned int shadowTextureUnitOffset = 0;
  std::vector<glm::mat4> lightProjections;
  std::vector<glm::mat4> perspective;
  glm::vec3 lightPos;
  glm::vec3 target{0.0f, 0.0f, 0.0f};
  glm::vec3 up{0.0f, 1.0f, 0.0f};

  unsigned int width = 4096;
  unsigned int height = 4096;
  unsigned int cascadeCount;

  std::vector<std::unique_ptr<ge::gl::Texture>> depthMaps;

  ge::gl::Framebuffer depthMapFBO;

  std::shared_ptr<ge::gl::Program> program = std::make_shared<ge::gl::Program>("shadow_map/sm"_vert, "shadow_map/sm"_frag);

  glm::mat4 calculateOrthoMatrices(const glm::mat4 &cameraView,
                                   float cameraNear, float cameraFar,
                                   float aspectRatio, float fieldOfView);

  void bindCascade(unsigned int index);
};

template <typename F>
void CascadedShadowMap::renderShadowMap(F renderFunction,
                                        const glm::mat4 &cameraView,
                                        float cameraNear, float cameraFar,
                                        float aspectRatio, float fieldOfView) {
  using namespace MakeRange;
  calculateOrthoMatrices(cameraView, cameraNear, cameraFar, aspectRatio,
                         fieldOfView);
  program->use();
  depthMapFBO.bind(GL_FRAMEBUFFER);
  for (auto i : range(cascadeCount)) {
    bindCascade(i);
    ge::gl::glClear(GL_DEPTH_BUFFER_BIT);
    const auto lightSpaceMatrix =
        perspective[i] * glm::lookAt(lightPos, target, up);
    program->setMatrix4fv("lightSpaceMatrix", &lightSpaceMatrix[0][0]);
    renderFunction(program);
  }
  depthMapFBO.unbind(GL_FRAMEBUFFER);
}

#endif // TERRAINGENERATION_CASCADEDSHADOWMAP_H
