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

  struct vertex_t {
    vertex_t(float _x, float _y, float _z, float _s, float _t, float _r,
             float _g, float _b, float _a)
        : x(_x), y(_y), z(_z), s(_s), t(_t), r(_r), g(_g), b(_b), a(_a) {}

    float x, y, z;     // position
    float s, t;        // texture
    float r, g, b, a;  // color
  };

  explicit Text(FontManager &fontManager, const std::wstring &initialValue,
                Font *font, float fontSize);

public:
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
