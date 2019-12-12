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

  [[nodiscard]] const glm::vec3 &getLightDir() const;
  void setLightDir(const glm::vec3 &lightDir);
  const glm::vec3 &getLightPos() const;
  void setLightPos(const glm::vec3 &lightPos);

  [[nodiscard]] const std::vector<std::unique_ptr<ge::gl::Texture>> &getDepthMaps() const;

  [[nodiscard]] glm::mat4 lightSpaceMatrix(unsigned int cascade) const;

  template <typename F>
  void renderShadowMap(F renderFunction, const glm::mat4 &cameraView,
                       float cameraNear, float cameraFar, float aspectRatio,
                       float fieldOfView);

private:
  static constexpr unsigned int shadowTextureUnitOffset = 0;
  std::vector<glm::mat4> lightProjections;
  std::vector<glm::mat4> perspective;
  glm::vec3 lightDir;
  glm::vec3 lightPos;
  float lightHeight = 15;

  unsigned int width = 1920;
  unsigned int height = 1080;
  unsigned int cascadeCount;

  std::vector<std::unique_ptr<ge::gl::Texture>> depthMaps;

  ge::gl::Framebuffer depthMapFBO;

  std::shared_ptr<ge::gl::Program> program = std::make_shared<ge::gl::Program>("shadow_map/sm"_vert, "shadow_map/sm"_frag);

  glm::mat4 calculateOrthoMatrices(const glm::mat4 &cameraView,
                                   float cameraNear, float cameraFar,
                                   float aspectRatio, float fieldOfView);

  void bindCascade(unsigned int index);

  [[nodiscard]] glm::mat4 calcLightView() const;
  GLint m_viewport[4]{};

};

template <typename F>
void CascadedShadowMap::renderShadowMap(F renderFunction,
                                        const glm::mat4 &cameraView,
                                        float cameraNear, float cameraFar,
                                        float aspectRatio, float fieldOfView) {
  using namespace MakeRange;
  calculateOrthoMatrices(cameraView, cameraNear, cameraFar, aspectRatio,
                         fieldOfView);
  ge::gl::glGetIntegerv(GL_VIEWPORT, m_viewport);

  program->use();
  ge::gl::glViewport(0, 0, width, height);
  depthMapFBO.bind(GL_FRAMEBUFFER);
  for (auto i : range(cascadeCount)) {
    bindCascade(i);
    ge::gl::glClear(GL_DEPTH_BUFFER_BIT);
    const auto lightSpaceMatrix =
        perspective[i] *  calcLightView();
    program->setMatrix4fv("lightSpaceMatrix", &lightSpaceMatrix[0][0]);
    renderFunction(program);
  }
  depthMapFBO.unbind(GL_FRAMEBUFFER);
  auto [x, y, width, height] = m_viewport;
  ge::gl::glViewport(x, y, width, height);
}

#endif // TERRAINGENERATION_CASCADEDSHADOWMAP_H
