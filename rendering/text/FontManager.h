//
// Created by petr on 11/22/19.
//

#ifndef TERRAINGENERATION_FONTMANAGER_H
#define TERRAINGENERATION_FONTMANAGER_H

#include <String.h>
#include <freetype-gl++/texture-font+.hpp>
#include <glm/glm.hpp>
#include <unordered_map>
#include <utility>

class FontManager {
  friend class TextRenderer;

public:
  explicit FontManager(String path, glm::uvec3 atlasSize = {512, 512, 1})
      : folder(std::move(path)), atlas(new freetypeglxx::TextureAtlas(
                                     atlasSize.x, atlasSize.y, atlasSize.z)) {
    folder += "/{}.ttf";
  }

  void loadFont(const String &name, float size) {
    auto font = std::unique_ptr<freetypeglxx::TextureFont>(
        new freetypeglxx::TextureFont(atlas.get(), folder.format(name), size));
    fonts[name.c_str() + std::to_string(size)] = std::move(font);
  }

  bool hasFont(const String &name, float size) {
    return fonts.find(name.c_str() + std::to_string(size)) != fonts.end();
  }

  freetypeglxx::TextureFont *getFont(const String &name, float size) {
    if (!hasFont(name, size)) {
      loadFont(name, size);
    }
    return fonts[name.c_str() + std::to_string(size)].get();
  }

private:
  String folder;
  std::unordered_map<std::string, std::unique_ptr<freetypeglxx::TextureFont>>
      fonts;
  std::unique_ptr<freetypeglxx::TextureAtlas> atlas;
};

#endif // TERRAINGENERATION_FONTMANAGER_H
