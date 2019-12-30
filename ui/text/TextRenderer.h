//
// Created by petr on 11/22/19.
//

#ifndef TERRAINGENERATION_TEXTRENDERER_H
#define TERRAINGENERATION_TEXTRENDERER_H

#include "FontManager.h"
#include "Text.h"
#include <freetype-gl++/shader+.hpp>
#include <freetype-gl++/vertex-buffer+.hpp>
#include <observable/observable.hpp>

class TextRenderer {
public:
  explicit TextRenderer(const String &fontPath, glm::uvec3 atlasSize = {4096, 4096, 1});

  void begin(glm::mat4 projection, glm::mat4 view, glm::mat4 model);

  void render(Text &text);

  void end();

  Text createText(const std::wstring &initialValue = L"", Font *font = nullptr, float fontSize = 10);

  FontManager &getFontManager();

private:
  GLuint shader;
  FontManager fontManager;
};

#endif // TERRAINGENERATION_TEXTRENDERER_H
