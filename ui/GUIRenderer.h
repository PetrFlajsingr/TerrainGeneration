//
// Created by petr on 11/20/19.
//

#ifndef TERRAINGENERATION_GUIRENDERER_H
#define TERRAINGENERATION_GUIRENDERER_H

#include "shader_literals.h"
#include <String.h>
#include <geGL/Program.h>

namespace sdl2cpp::ui {
const String tmpPath{
    "/home/petr/CLionProjects/TerrainGeneration/assets/gui/fonts/{}"};
class GUIRenderer {
  using Program = std::shared_ptr<ge::gl::Program>;

public:
  GUIRenderer();

  Program getProgram();

private:
  Program drawProgram;
};

} // namespace sdl2cpp::ui

#endif // TERRAINGENERATION_GUIRENDERER_H
