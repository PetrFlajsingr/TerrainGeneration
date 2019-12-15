//
// Created by petr on 11/22/19.
//

#include "Font.h"
#include "rendering/text/FontManager.h"
#include <algorithm>

Font::Font(std::string name, FontManager &fontManager)
    : name(std::move(name)), fontManager(fontManager) {}

freetypeglxx::TextureFont *Font::getData(float size) {
  if (auto iter =
          std::find_if(fontSizes.begin(), fontSizes.end(),
                       [size](const auto &val) { return val.first == size; });
      iter != fontSizes.end()) {
    return iter->second.get();
  }
  fontSizes.emplace_back(size, fontManager.load(name, size));
  return fontSizes.back().second.get();
}
Font::Font(Font &&other) noexcept
    : fontSizes(std::move(other.fontSizes)), fontManager(other.fontManager) {}
