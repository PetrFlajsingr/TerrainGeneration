//
// Created by petr on 12/8/19.
//

#ifndef TERRAINGENERATION_CASCADEDSHADOWMAP_H
#define TERRAINGENERATION_CASCADEDSHADOWMAP_H
#include <glm/glm.hpp>
#include <geGL/geGL.h>
#include <geGL_utils.h>
using namespace ShaderLiterals;
class CascadedShadowMap {
public:
  CascadedShadowMap(unsigned int cascadeCount, unsigned int textureWidth, unsigned int textureHeight);

  void bindCascade(unsigned int index);

  void bindRender();

private:
  static constexpr unsigned int shadowTextureUnitOffset = 0;
  std::vector<glm::mat4> lightProjections;

  glm::mat4 lightSpaceMatrix;
  glm::vec3 lightPos;
  glm::vec3 target{0.0f, 0.0f, 0.0f};
  glm::vec3 up{0.0f, 1.0f, 0.0f};
  GLint m_viewport[4]{};

  unsigned int width = 4096;
  unsigned int height = 4096;
  unsigned int cascadeCount;

  std::vector<ge::gl::Texture> depthMaps;

  ge::gl::Framebuffer depthMapFBO;

  std::shared_ptr<ge::gl::Program> program = std::make_shared<ge::gl::Program>(
      "shadow_map/sm"_vert, "shadow_map/sm"_frag);
};

#endif // TERRAINGENERATION_CASCADEDSHADOWMAP_H
