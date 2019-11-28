//
// Created by petr on 11/25/19.
//

#include "Text.h"

Text::Text(FontManager &fontManager, const std::wstring &initialValue,
           Font *font, float fontSize)
    : fontManager(fontManager), font(font), fontSize(fontSize) {}

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
  if (text == Text::text.get()) {
    return;
  }
  Text::text.set(text);
  calcText(text, color, position);
}

void Text::calcText(const WString &str, glm::vec4 color, glm::vec3 pen) {
  if (font == nullptr) {
    return;
  }
  buffer.Clear();
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
      buffer.PushBack(vertices, indices);
      pen.x += glyph->advance_x();
    }
  }
}
void Text::setFont(const std::string &name) {
  font = &fontManager.getFont(name);
  calcText(text.get(), color, position);
}
void Text::setFont(const std::string &name, float size) {
  font = &fontManager.getFont(name);
  fontSize = size;
  calcText(text.get(), color, position);
}