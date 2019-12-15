//
// Created by petr on 12/5/19.
//

#include "DrawTexture.h"
#include <geGL/StaticCalls.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

DrawTexture::DrawTexture()
    : quadVertices({
          -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
          1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
      }),
      quadVBO(sizeof(float) * quadVertices.size(), quadVertices.data()) {
  quadVAO.addAttrib(&quadVBO, 0, 3, GL_FLOAT, 5 * sizeof(float), 0);
  quadVAO.addAttrib(&quadVBO, 1, 2, GL_FLOAT, 5 * sizeof(float),
                    (3 * sizeof(float)));
}
void DrawTexture::draw(GLint texture) {
  program.use();
  quadVAO.bind();
  ge::gl::glBindTexture(GL_TEXTURE_2D, texture);
  ge::gl::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
void DrawTexture::drawCasc(GLuint texture) {
  programCasc.use();
  ge::gl::glBindFramebuffer(GL_FRAMEBUFFER, 0);
  ge::gl::glClear(GL_DEPTH_BUFFER_BIT);
  ge::gl::glUniform1i(programCasc.getUniformLocation("arrayTex"), 0);
  ge::gl::glUniform1i(programCasc.getUniformLocation("layer"), 0);

  glm::mat4 modelMatrix = glm::mat4(1.0f);
  modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 0.25f));
  modelMatrix = glm::translate(modelMatrix, glm::vec3(-2.9f, 2.9f, 1.0f));
  ge::gl::glUniformMatrix4fv(programCasc.getUniformLocation("modelMatrix"), 1,
                             GL_FALSE, &modelMatrix[0][0]);
  ge::gl::glActiveTexture(GL_TEXTURE1);
  ge::gl::glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE,
                          GL_NONE);
  ge::gl::glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
  ge::gl::glBlendEquation(GL_FUNC_ADD);
  ge::gl::glBlendFunc(GL_ONE, GL_ONE);

  ge::gl::glBindVertexArray(quadVAO.getId());
  ge::gl::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  ge::gl::glBindVertexArray(0);

  ge::gl::glUniform1i(programCasc.getUniformLocation("layer"), 1);
  modelMatrix = glm::mat4(1.0f);
  modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 0.25f));
  modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 2.9f, 1.0f));
  ge::gl::glUniformMatrix4fv(programCasc.getUniformLocation("modelMatrix"), 1,
                             GL_FALSE, &modelMatrix[0][0]);
  ge::gl::glActiveTexture(GL_TEXTURE1);
  ge::gl::glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
  ge::gl::glBlendEquation(GL_FUNC_ADD);
  ge::gl::glBlendFunc(GL_ONE, GL_ONE);

  ge::gl::glBindVertexArray(quadVAO.getId());
  ge::gl::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  ge::gl::glBindVertexArray(0);

  ge::gl::glUniform1i(programCasc.getUniformLocation("layer"), 2);
  modelMatrix = glm::mat4(1.0f);
  modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 0.25f));
  modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -2.9f, 1.0f));
  ge::gl::glUniformMatrix4fv(programCasc.getUniformLocation("modelMatrix"), 1,
                             GL_FALSE, &modelMatrix[0][0]);
  ge::gl::glActiveTexture(GL_TEXTURE1);
  ge::gl::glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
  ge::gl::glBlendEquation(GL_FUNC_ADD);
  ge::gl::glBlendFunc(GL_ONE, GL_ONE);

  ge::gl::glBindVertexArray(quadVAO.getId());
  ge::gl::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  ge::gl::glBindVertexArray(0);

  ge::gl::glUniform1i(programCasc.getUniformLocation("layer"), 3);
  modelMatrix = glm::mat4(1.0f);
  modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 0.25f));
  modelMatrix = glm::translate(modelMatrix, glm::vec3(-2.9f, -2.9f, 1.0f));
  ge::gl::glUniformMatrix4fv(programCasc.getUniformLocation("modelMatrix"), 1,
                             GL_FALSE, &modelMatrix[0][0]);
  ge::gl::glActiveTexture(GL_TEXTURE1);
  ge::gl::glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
  ge::gl::glBlendEquation(GL_FUNC_ADD);
  ge::gl::glBlendFunc(GL_ONE, GL_ONE);

  ge::gl::glBindVertexArray(quadVAO.getId());
  ge::gl::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  ge::gl::glBindVertexArray(0);
  ge::gl::glUseProgram(0);
}
