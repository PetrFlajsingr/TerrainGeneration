//
// Created by petr on 1/3/20.
//

#include "MCTextures.h"
#include "utils/types/Range.h"
#include <geGL/StaticCalls.h>
#include <rendering/textures/FileTextureLoader.h>

using namespace MakeRange;

MarchingCubesTextures::MarchingCubesTextures(const std::string &assetPath, const std::array<std::string, 6> &files) {
  FileTextureLoader textureLoader{assetPath};
  TexOptions texOptions{GL_TEXTURE_2D,
                        GL_RGB,
                        true,
                        {
                            {GL_TEXTURE_WRAP_S, GL_REPEAT},
                            {GL_TEXTURE_WRAP_T, GL_REPEAT},
                            {GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR},
                            {GL_TEXTURE_MAG_FILTER, GL_LINEAR},
                        }};
  for (auto i : range(6)) {
    textures[i] = textureLoader.loadTexture(files[i], texOptions);
  }
}

void MarchingCubesTextures::bind(ge::gl::Program &program) {
  for (auto i : range(6)) {
    ge::gl::glActiveTexture(GL_TEXTURE0 + 1 + i);
    ge::gl::glBindTexture(GL_TEXTURE_2D, textures[i]);
  }
  ge::gl::glUniform1i(program.getUniformLocation("texturePlusX"), 1);
  ge::gl::glUniform1i(program.getUniformLocation("textureMinusX"), 2);
  ge::gl::glUniform1i(program.getUniformLocation("texturePlusY"), 3);
  ge::gl::glUniform1i(program.getUniformLocation("textureMinusY"), 4);
  ge::gl::glUniform1i(program.getUniformLocation("texturePlusZ"), 5);
  ge::gl::glUniform1i(program.getUniformLocation("textureMinusZ"), 6);
}

MarchingCubesTextures::~MarchingCubesTextures() {
  ge::gl::glDeleteTextures(6, textures.data());
}
