//
// Created by petr on 12/12/19.
//

#ifndef TERRAINGENERATION_SCENELOADER_H
#define TERRAINGENERATION_SCENELOADER_H

#include "rendering/models/ModelRenderer.h"
#include <memory>
#include <string>
#include <tinyxml2.h>
#include <vector>

class SceneLoader {
public:
  explicit SceneLoader(const std::string &assetsPath,
                       const std::string &sceneName);

  [[nodiscard]] bool isValid() const;

  class iterator {
  public:
    using iterator_category = std::input_iterator_tag;
    using value_type = std::vector<std::shared_ptr<GraphicsModelBase>>;
    using difference_type = int;
    using pointer = value_type *;
    using reference = value_type &;

    iterator() = default;
    explicit iterator(SceneLoader &loader, bool isEnd = false);
    iterator(const iterator &other);
    iterator &operator=(const iterator &other);
    bool operator==(const iterator &rhs) const;
    bool operator!=(const iterator &rhs) const;

    iterator &operator++();

    value_type operator*();

  private:
    value_type currentModels;
    SceneLoader *loader = nullptr;
  };

  iterator begin();

  iterator end();

private:
  bool valid = true;
  tinyxml2::XMLDocument doc;
  tinyxml2::XMLNode *xmlIter;

  std::vector<std::shared_ptr<GraphicsModelBase>> getNextModel();

  ObjModelLoader modelLoader;
};

#endif // TERRAINGENERATION_SCENELOADER_H
