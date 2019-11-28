//
// Created by petr on 11/22/19.
//

#ifndef TERRAINGENERATION_TEXTRENDERER_H
#define TERRAINGENERATION_TEXTRENDERER_H

#include "FontManager.h"
#include "Text.h"
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


class TextRenderer {
public:
  explicit TextRenderer(const String &fontPath,
                        glm::uvec3 atlasSize = {4096, 4096, 1});

  void begin(glm::mat4 projection, glm::mat4 view, glm::mat4 model);

  void render(Text &text);

  void end();

  FontManager &getFontManager();

private:
  GLuint shader;
  FontManager fontManager;
};

#endif // TERRAINGENERATION_TEXTRENDERER_H
