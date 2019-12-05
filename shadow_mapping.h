//
// Created by petr on 11/30/19.
//

#ifndef TERRAINGENERATION_SHADOW_MAPPING_H
#define TERRAINGENERATION_SHADOW_MAPPING_H

#include "utils/geGL_utils.h"
#include <geGL/geGL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std::string_literals;
using namespace ShaderLiterals;

struct DrawFrameBuffer {
  unsigned int quadVAO, quadVBO;
  ge::gl::Program program{"framebuffer_screen"_vert, "framebuffer_screen"_frag};
  DrawFrameBuffer();
};

struct ShadowMapData {
  const GLsizei width = 4096;
  const GLsizei height = 4096;

  const float near_plane = 1.0f;
  const float far_plane = 70.5f;
  const glm::mat4 lightProjection =glm::perspective(glm::radians(140.f), (float)1024 / (float)1024, 0.1f, 100.0f);
     // glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
  // std::shared_ptr<ge::gl::Texture> depthMap =
  // std::make_shared<ge::gl::Texture>(GL_TEXTURE_2D, GL_DEPTH_COMPONENT, 0,
  // width, height);

  unsigned int depthMap;
  unsigned int depthMapFBO;
  // ge::gl::Framebuffer fbo;

  glm::vec3 lightPos;
  glm::vec3 target{0.0f, 0.0f, 0.0f};
  glm::vec3 up{0.0f, 1.0f, 0.0f};

  glm::mat4 lightSpaceMatrix;

  std::shared_ptr<ge::gl::Program> program = std::make_shared<ge::gl::Program>(
      "shadow_map/sm"_vert, "shadow_map/sm"_frag);
  std::shared_ptr<ge::gl::Program> renderProgram =
      std::make_shared<ge::gl::Program>("shadow_map/render"_vert,
                                        "shadow_map/render"_frag);
  ShadowMapData();

  GLint m_viewport[4]{};
  void begin();

  void end();
};

void main_shadow_mapping(int argc, char *argv[]);

#endif // TERRAINGENERATION_SHADOW_MAPPING_H
