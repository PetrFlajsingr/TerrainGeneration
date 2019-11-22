//
// Created by petr on 11/22/19.
//

#include "TextRenderer.h"

const glm::vec4 &Text::getColor() const { return color; }
void Text::setColor(const glm::vec4 &color) {
  Text::color = color;
  calcText(text.get(), color, position);
}
const glm::vec3 &Text::getPosition() const { return position; }
void Text::setPosition(const glm::vec3 &position) {
  Text::position = position;
  calcText(text.get(), color, position);
}
Text::Text(const std::wstring &initialValue, Font *font, float fontSize)
    : font(font), fontSize(fontSize) {}
Font &Text::getFont() const { return *font; }
void Text::setFont(Font &font) {
  Text::font = &font;
  calcText(text.get(), color, position);
}
float Text::getFontSize() const { return fontSize; }
void Text::setFontSize(float fontSize) {
  Text::fontSize = fontSize;
  calcText(text.get(), color, position);
}
void Text::setText(const WString &text) {
  Text::text.set(text);
  calcText(text, color, position);
}
void Text::calcText(const WString &str, glm::vec4 color, glm::vec3 pen) {
  if (font == nullptr) {
    return;
  }
  buffer->Clear();
  size_t i;
  float r = color.r, g = color.g, b = color.b, a = color.a;
  GLuint indices_raw[6] = {0, 1, 2, 0, 2, 3};
  std::vector<GLuint> indices(6);
  memcpy(indices.data(), indices_raw, sizeof(indices_raw));
  for (i = 0; i < wcslen(str.c_str()); ++i) {
    freetypeglxx::TextureGlyph *glyph =
        font->getData(fontSize)->GetGlyph(str.c_str()[i]);
    if (glyph != NULL) {
      int kerning = 0;
      if (i > 0) {
        kerning = glyph->GetKerning(str.c_str()[i - 1]);
      }
      pen.x += kerning;
      int x0 = (int)(pen.x + glyph->offset_x());
      int y0 = (int)(pen.y + glyph->offset_y());
      int x1 = (int)(x0 + glyph->width());
      int y1 = (int)(y0 - glyph->height());
      std::vector<vertex_t> vertices;
      vertices.emplace_back(
          vertex_t(x0, y0, pen.z, glyph->s0(), glyph->t0(), r, g, b, a));
      vertices.emplace_back(
          vertex_t(x0, y1, pen.z, glyph->s0(), glyph->t1(), r, g, b, a));
      vertices.emplace_back(
          vertex_t(x1, y1, pen.z, glyph->s1(), glyph->t1(), r, g, b, a));
      vertices.emplace_back(
          vertex_t(x1, y0, pen.z, glyph->s1(), glyph->t0(), r, g, b, a));
      buffer->PushBack(vertices, indices);
      pen.x += glyph->advance_x();
    }
  }
}

TextRenderer::TextRenderer(const String &fontPath, glm::uvec3 atlasSize)
    : fontManager(fontPath, atlasSize) {
  shader = freetypeglxx::shader::Load(
      "/home/petr/CLionProjects/TerrainGeneration/shaders/v3f-t2f-c4f.vert",
      "/home/petr/CLionProjects/TerrainGeneration/shaders/v3f-t2f-c4f.frag");
}
void TextRenderer::begin(glm::mat4 projection, glm::mat4 view,
                         glm::mat4 model) {
  ge::gl::glUseProgram(shader);
  ge::gl::glEnable(GL_BLEND);
  ge::gl::glDisable(GL_DEPTH_TEST);
  ge::gl::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  ge::gl::glBindTexture(GL_TEXTURE_2D, fontManager.atlas->id());

  ge::gl::glUniform1i(ge::gl::glGetUniformLocation(shader, "texture"), 0);
  ge::gl::glUniformMatrix4fv(ge::gl::glGetUniformLocation(shader, "model"), 1,
                             0, &model[0][0]);
  ge::gl::glUniformMatrix4fv(ge::gl::glGetUniformLocation(shader, "view"), 1, 0,
                             &view[0][0]);
  ge::gl::glUniformMatrix4fv(ge::gl::glGetUniformLocation(shader, "projection"),
                             1, 0, &projection[0][0]);
}
void TextRenderer::render(Text &text) { text.buffer->Render(GL_TRIANGLES); }
void TextRenderer::end() { ge::gl::glDisable(GL_BLEND); }
FontManager &TextRenderer::getFontManager() { return fontManager; }
