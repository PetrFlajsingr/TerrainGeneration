//
// Created by petr on 11/22/19.
//

#ifndef TERRAINGENERATION_FONT_H
#define TERRAINGENERATION_FONT_H

#include <freetype-gl++/texture-font+.hpp>
#include <memory>
#include <string>
#include <utility>
#include <vector>

class FontManager;
class Font {
  using FontData = std::unique_ptr<freetypeglxx::TextureFont>;
  using FontSizeMap = std::vector<std::pair<float, FontData>>;

public:
  explicit Font(std::string name, FontManager &fontManager);
  Font(Font &&other) noexcept;
  freetypeglxx::TextureFont *getData(float size);
  const std::string name;

private:
  FontSizeMap fontSizes;
  FontManager &fontManager;
};

#endif // TERRAINGENERATION_FONT_H
