//
// Created by petr on 12/27/19.
//

#include "FileTextureLoader.h"
FileTextureLoader::FileTextureLoader(std::string assetsPath) : assetsPath(assetsPath + "/textures/") {}

GLuint FileTextureLoader::loadTexture(const std::string &name, const TexOptions &texOptions) {
  loc_assert(texOptions.target == GL_TEXTURE_2D, "No support for not 2D textures yet");
  loc_assert(isIn(texOptions.internalFormat, {GL_RGB, GL_RGBA}));
  int desiredFormat = texOptions.internalFormat == GL_RGBA ? STBI_rgb_alpha : STBI_rgb;
  int width, height, channels;
  unsigned char *image1 = stbi_load((assetsPath + name).c_str(), &width, &height, &channels, desiredFormat);

  GLuint result;
  ge::gl::glGenTextures(1, &result);
  ge::gl::glBindTexture(GL_TEXTURE_2D, result);
  for (const auto &parI : texOptions.parI) {
    ge::gl::glTexParameteri(texOptions.target, parI.pName, parI.param);
  }
  ge::gl::glTexImage2D(texOptions.target, 0, texOptions.internalFormat, width, height, 0, texOptions.internalFormat,
                       GL_UNSIGNED_BYTE, image1);
  if (texOptions.useMipMaps) {
    ge::gl::glGenerateMipmap(GL_TEXTURE_2D);
  }
  stbi_image_free(image1);
  return result;
}
