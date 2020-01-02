//
// Created by petr on 12/27/19.
//

#ifndef TERRAINGENERATION_FILETEXTURELOADER_H
#define TERRAINGENERATION_FILETEXTURELOADER_H

#include <geGL/StaticCalls.h>
#include <stb_image.h>
#include <string>
#include <utility>
#include <various/isin.h>
#include <various/loc_assert.h>
#include <vector>

struct TexParI {
  GLenum pName;
  GLint param;
};

struct TexOptions {
  GLenum target;
  GLint internalFormat;
  bool useMipMaps;
  std::vector<TexParI> parI;
};

class FileTextureLoader {
public:
  explicit FileTextureLoader(std::string assetsPath);

  [[nodiscard]] GLuint loadTexture(const std::string &name, const TexOptions &texOptions);

private:
  std::string assetsPath;
};

#endif // TERRAINGENERATION_FILETEXTURELOADER_H
