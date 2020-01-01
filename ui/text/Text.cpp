//
// Created by petr on 11/25/19.
//

#include "Text.h"

Text::Text(FontManager &fontManager, const std::wstring &initialValue, Font *font, float fontSize)
    : font(font), fontSize(fontSize), fontManager(fontManager) {
  text = WString{initialValue};
}

const glm::vec4 &Text::getColor() const { return color; }

Text & Text::setColor(const glm::vec4 &color) {
  Text::color = color;
  calcText(text.get(), color, position);
  return *this;
}

const glm::vec3 &Text::getPosition() const { return position; }

Text & Text::setPosition(const glm::vec3 &position) {
  Text::position = position;
  calcText(text.get(), color, position);
  return *this;
}

Font &Text::getFont() const { return *font; }

Text & Text::setFont(Font &font) {
  Text::font = &font;
  calcText(text.get(), color, position);
  return *this;
}

float Text::getFontSize() const { return fontSize; }

Text & Text::setFontSize(float fontSize) {
  Text::fontSize = fontSize;
  calcText(text.get(), color, position);
  return *this;
}

Text & Text::setText(const WString &text) {
  if (text != Text::text.get()) {
    Text::text.set(text);
    calcText(text, color, position);
  }
  return *this;
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
    freetypeglxx::TextureGlyph *glyph = font->getData(fontSize)->GetGlyph(str.c_str()[i]);
    if (glyph != nullptr) {
      int kerning = 0;
      if (i > 0) {
        kerning = glyph->GetKerning(str.c_str()[i - 1]);
      }
      pen.x += kerning;
      int x0 = static_cast<int>(pen.x + glyph->offset_x());
      int y0 = static_cast<int>(pen.y + glyph->offset_y());
      int x1 = static_cast<int>(x0 + glyph->width());
      int y1 = static_cast<int>(y0 - glyph->height());
      std::vector<vertex_t> vertices;
      vertices.emplace_back(
          vertex_t(static_cast<float>(x0), static_cast<float>(y0), pen.z, glyph->s0(), glyph->t0(), r, g, b, a));
      vertices.emplace_back(
          vertex_t(static_cast<float>(x0), static_cast<float>(y1), pen.z, glyph->s0(), glyph->t1(), r, g, b, a));
      vertices.emplace_back(
          vertex_t(static_cast<float>(x1), static_cast<float>(y1), pen.z, glyph->s1(), glyph->t1(), r, g, b, a));
      vertices.emplace_back(
          vertex_t(static_cast<float>(x1), static_cast<float>(y0), pen.z, glyph->s1(), glyph->t0(), r, g, b, a));
      buffer.PushBack(vertices, indices);
      pen.x += glyph->advance_x();
    }
  }
}
Text & Text::setFont(const std::string &name) {
  font = &fontManager.getFont(name);
  calcText(text.get(), color, position);
  return *this;
}
Text & Text::setFont(const std::string &name, float size) {
  font = &fontManager.getFont(name);
  fontSize = size;
  calcText(text.get(), color, position);
  return *this;
}
