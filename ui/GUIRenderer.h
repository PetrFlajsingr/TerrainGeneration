//
// Created by petr on 11/20/19.
//

#ifndef TERRAINGENERATION_GUIRENDERER_H
#define TERRAINGENERATION_GUIRENDERER_H

#include "rendering/text/TextRenderer.h"
#include <String.h>
#include <memory>
#include <geGL/Program.h>

namespace sdl2cpp::ui {

class GUIRenderer {
  using Program = std::shared_ptr<ge::gl::Program>;

public:
  explicit GUIRenderer(const String &fontsPath);

  Program getProgram();
  TextRenderer &getTextRenderer() { return textRenderer; }

private:
  Program drawProgram;
  TextRenderer textRenderer;
};

} // namespace sdl2cpp::ui

#endif // TERRAINGENERATION_GUIRENDERER_H
