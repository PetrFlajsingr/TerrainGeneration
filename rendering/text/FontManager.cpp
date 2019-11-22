//
// Created by petr on 11/22/19.
//

#include "FontManager.h"

FontManager::FontManager(String path, glm::uvec3 atlasSize)
    : folder(std::move(path)), atlas(new freetypeglxx::TextureAtlas(
                                   atlasSize.x, atlasSize.y, atlasSize.z)) {
  folder += "/{}.ttf";
}
void FontManager::loadFont(const std::string &name) {
  fonts.try_emplace(name, name, *this);
}
Font &FontManager::getFont(const std::string &name) {
  if (!hasFont(name)) {
    loadFont(name);
  }
  return fonts.at(name);
}

std::unique_ptr<freetypeglxx::TextureFont>
FontManager::load(const std::string &name, float size) {
  print("Loading font: ", name, " ", size);
  return std::unique_ptr<freetypeglxx::TextureFont>(
      new freetypeglxx::TextureFont(atlas.get(), folder.format(name), size));
}