//
// Created by petr on 11/25/19.
//

#ifndef TERRAINGENERATION_TEXT_H
#define TERRAINGENERATION_TEXT_H

#include "FontManager.h"
#include "observable/observable.hpp"
#include <freetype-gl++/vertex-buffer+.hpp>

class Text {
  freetypeglxx::VertexBuffer buffer =
      freetypeglxx::VertexBuffer("vertex:3f,tex_coord:2f,color:4f");
  OBSERVABLE_PROPERTIES(Text);
  friend class TextRenderer;

public:
  explicit Text(FontManager &fontManager,
                const std::wstring &initialValue = L"", Font *font = nullptr,
                float fontSize = 10);

  [[nodiscard]] Font &getFont() const;
  void setFont(Font &font);
  void setFont(const std::string &name);
  void setFont(const std::string &name, float size);

  [[nodiscard]] float getFontSize() const;
  void setFontSize(float fontSize);

  void setText(const WString &text);

  const glm::vec4 &getColor() const;
  void setColor(const glm::vec4 &color);
  const glm::vec3 &getPosition() const;
  void setPosition(const glm::vec3 &position);

  observable_property<WString> text;

private:
  Font *font;
  float fontSize;
  glm::vec4 color;
  glm::vec3 position;

  FontManager &fontManager;

  void calcText(const WString &str, glm::vec4 color, glm::vec3 pen);
};

#endif // TERRAINGENERATION_TEXT_H
