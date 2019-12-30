//
// Created by petr on 11/22/19.
//

#include "TextRenderer.h"

TextRenderer::TextRenderer(const String &fontPath, glm::uvec3 atlasSize) : fontManager(fontPath, atlasSize) {
  shader = freetypeglxx::shader::Load("/home/petr/CLionProjects/TerrainGeneration/shaders/v3f-t2f-c4f.vert",
                                      "/home/petr/CLionProjects/TerrainGeneration/shaders/v3f-t2f-c4f.frag");
}

void TextRenderer::begin(glm::mat4 projection, glm::mat4 view, glm::mat4 model) {
  ge::gl::glUseProgram(shader);
  ge::gl::glEnable(GL_BLEND);
  ge::gl::glDisable(GL_DEPTH_TEST);
  ge::gl::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  ge::gl::glBindTexture(GL_TEXTURE_2D, fontManager.atlas->id());

  ge::gl::glUniform1i(ge::gl::glGetUniformLocation(shader, "texture"), 0);
  ge::gl::glUniformMatrix4fv(ge::gl::glGetUniformLocation(shader, "model"), 1, 0, &model[0][0]);
  ge::gl::glUniformMatrix4fv(ge::gl::glGetUniformLocation(shader, "view"), 1, 0, &view[0][0]);
  ge::gl::glUniformMatrix4fv(ge::gl::glGetUniformLocation(shader, "projection"), 1, 0, &projection[0][0]);
}

void TextRenderer::render(Text &text) { text.buffer.Render(GL_TRIANGLES); }

void TextRenderer::end() { ge::gl::glDisable(GL_BLEND); }

FontManager &TextRenderer::getFontManager() { return fontManager; }

Text TextRenderer::createText(const std::wstring &initialValue, Font *font, float fontSize) {
  return Text{fontManager, initialValue, font, fontSize};
}
