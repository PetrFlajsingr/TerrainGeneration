//
// Created by petr on 1/3/20.
//

#ifndef TERRAINGENERATION_MCTEXTURES_H
#define TERRAINGENERATION_MCTEXTURES_H

#include <string>
#include <array>
#include <geGL/Program.h>

class MarchingCubesTextures {
public:
  MarchingCubesTextures(const std::string &assetPath, const std::array<std::string, 6> &files);
  virtual ~MarchingCubesTextures();

  void bind(ge::gl::Program &program);

private:
  std::array<GLuint, 6> textures{};
};

#endif // TERRAINGENERATION_MCTEXTURES_H
