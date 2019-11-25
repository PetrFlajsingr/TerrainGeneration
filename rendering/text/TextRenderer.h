//
// Created by petr on 11/22/19.
//

#ifndef TERRAINGENERATION_TEXTRENDERER_H
#define TERRAINGENERATION_TEXTRENDERER_H

#include "FontManager.h"
#include <freetype-gl++/shader+.hpp>
#include <freetype-gl++/vertex-buffer+.hpp>
#include <observable/value.hpp>

struct vertex_t {
  vertex_t(double _x, double _y, double _z, double _s, double _t, double _r,
           double _g, double _b, double _a)
      : x(_x), y(_y), z(_z), s(_s), t(_t), r(_r), g(_g), b(_b), a(_a) {}

  float x, y, z;    // position
  float s, t;       // texture
  float r, g, b, a; // color
};
class TextRenderer;

class Text {
  freetypeglxx::VertexBuffer buffer =
      freetypeglxx::VertexBuffer("vertex:3f,tex_coord:2f,color:4f");
  OBSERVABLE_PROPERTIES(Text);
  friend class TextRenderer;

public:
  Text(FontManager &fontManager, const std::wstring &initialValue = L"",
       Font *font = nullptr, float fontSize = 10);

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

class TextRenderer {
public:
  TextRenderer(const String &fontPath, glm::uvec3 atlasSize = {4096, 4096, 1});

  void begin(glm::mat4 projection, glm::mat4 view, glm::mat4 model);

  void render(Text &text);

  void end();

  FontManager &getFontManager();

private:
  GLuint shader;
  FontManager fontManager;
};

#endif // TERRAINGENERATION_TEXTRENDERER_H
