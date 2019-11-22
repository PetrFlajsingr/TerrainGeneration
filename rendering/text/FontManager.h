//
// Created by petr on 11/22/19.
//

#ifndef TERRAINGENERATION_FONTMANAGER_H
#define TERRAINGENERATION_FONTMANAGER_H

#include <String.h>
#include <freetype-gl++/texture-font+.hpp>
#include <glm/glm.hpp>
#include <io/print.h>
#include <ui/Font.h>
#include <unordered_map>
#include <utility>

class FontManager {
  friend class TextRenderer;
  friend class Font;

public:
  explicit FontManager(String path, glm::uvec3 atlasSize = {512, 512, 1});

  void loadFont(const std::string &name);

  bool hasFont(const std::string &name) {
    return fonts.find(name) != fonts.end();
  }

  Font &getFont(const std::string &name);

private:
  std::unordered_map<std::string, Font> fonts;
  String folder;
  std::unique_ptr<freetypeglxx::TextureAtlas> atlas;

  std::unique_ptr<freetypeglxx::TextureFont> load(const std::string &name,
                                                  float size);
};

#endif // TERRAINGENERATION_FONTMANAGER_H
