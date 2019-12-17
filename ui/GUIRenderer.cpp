//
// Created by petr on 11/20/19.
//

#include "GUIRenderer.h"
#include "graphics/shader_literals.h"

sdl2cpp::ui::GUIRenderer::Program sdl2cpp::ui::GUIRenderer::getProgram() {
  return drawProgram;
}
sdl2cpp::ui::GUIRenderer::GUIRenderer(const String &fontsPath)
    : textRenderer(fontsPath) {
  using namespace ShaderLiterals;
  auto fragShader = "uniform_color"_frag;
  drawProgram = std::make_shared<ge::gl::Program>(fragShader);
}
