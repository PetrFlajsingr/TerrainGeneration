//
// Created by petr on 11/30/19.
//

#ifndef TERRAINGENERATION_MARCHING_CUBES_H
#define TERRAINGENERATION_MARCHING_CUBES_H

#include <geGL/StaticCalls.h>
#include <geGL/geGL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "utils/geGL_utils.h"

using namespace std::string_literals;
using namespace ShaderLiterals;



struct Tmp {
  unsigned int quadVAO, quadVBO;
  ge::gl::Program program{"framebuffer_screen"_vert, "framebuffer_screen"_frag};
  Tmp();
};

struct SM {
  const GLsizei width = 1024;
  const GLsizei height = 1024;

  const float near_plane = .001f;
  const float far_plane = 1000.f;
  const glm::mat4 lightProjection = glm::ortho(-1000.0f, 1000.0f, -1000.0f, 1000.0f, near_plane, far_plane);
  //std::shared_ptr<ge::gl::Texture> depthMap = std::make_shared<ge::gl::Texture>(GL_TEXTURE_2D, GL_DEPTH_COMPONENT, 0, width, height);

  unsigned int depthMap;
  unsigned int depthMapFBO;
  //ge::gl::Framebuffer fbo;

  glm::vec3 lightPos;
  glm::vec3 target{500.0f, 450.0f,  500.0f};
  glm::vec3 up{0.0f, 1.0f,  0.0f};

  glm::mat4 lightSpaceMatrix;

  std::shared_ptr<ge::gl::Program> program= std::make_shared<ge::gl::Program>("shadow_map/sm"_vert, "shadow_map/sm"_frag);
  std::shared_ptr<ge::gl::Program> renderProgram = std::make_shared<ge::gl::Program>("shadow_map/render"_vert, "shadow_map/render"_frag);
  SM();

  GLint m_viewport[4]{};
  void begin();

  void end();
};

void main_marching_cubes(int argc, char *argv[]);

#endif // TERRAINGENERATION_MARCHING_CUBES_H
