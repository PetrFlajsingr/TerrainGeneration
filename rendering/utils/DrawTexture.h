//
// Created by petr on 12/5/19.
//

#ifndef TERRAINGENERATION_DRAWTEXTURE_H
#define TERRAINGENERATION_DRAWTEXTURE_H
#include "utils/geGL_utils.h"
#include <geGL/geGL.h>

using namespace ShaderLiterals;

struct DrawTexture {
  ge::gl::Program program{"framebuffer_screen"_vert, "framebuffer_screen"_frag};
  std::vector<float> quadVertices;
  ge::gl::Buffer quadVBO;
  ge::gl::VertexArray quadVAO;

public:
  DrawTexture();

  void draw(GLint texture);
};

#endif // TERRAINGENERATION_DRAWTEXTURE_H
