//
// Created by petr on 12/5/19.
//

#include "DrawTexture.h"
#include <geGL/StaticCalls.h>

DrawTexture::DrawTexture() : quadVertices({
                                              -10.0f, 10.0f, 0.0f, 0.0f, 10.0f, -10.0f, -10.0f, 0.0f, 0.0f, 0.0f,
                                              10.0f,  10.0f, 0.0f, 10.0f, 10.0f, 10.0f,  -10.0f, 0.0f, 10.0f, 0.0f,
                                          }), quadVBO(sizeof(float) * quadVertices.size(), quadVertices.data()) {
  quadVAO.addAttrib(&quadVBO, 0, 3, GL_FLOAT, 5 * sizeof(float), 0);
  quadVAO.addAttrib(&quadVBO, 1, 2, GL_FLOAT, 5 * sizeof(float), (3 * sizeof(float)));
}
void DrawTexture::draw(GLint texture) {
  program.use();
  quadVAO.bind();
  ge::gl::glBindTexture(GL_TEXTURE_2D, texture);
  ge::gl::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
