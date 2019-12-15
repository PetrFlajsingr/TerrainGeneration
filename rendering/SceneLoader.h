//
// Created by petr on 12/12/19.
//

#ifndef TERRAINGENERATION_SCENELOADER_H
#define TERRAINGENERATION_SCENELOADER_H

#include "ModelRenderer.h"
#include <string>
#include <memory>
#include <vector>
#include <tinyxml2.h>

class SceneLoader {
public:
  explicit SceneLoader(const std::string& assetsPath, const std::string& sceneName);

  [[nodiscard]] bool isValid() const;

  struct iterator {
    iterator() = default;
    explicit iterator(SceneLoader &loader, bool isEnd = false);
    iterator(const iterator &other);
    iterator &operator=(const iterator &other);
    bool operator==(const iterator &rhs) const;
    bool operator!=(const iterator &rhs) const;

    iterator &operator++();

    std::vector<std::shared_ptr<GraphicsModelBase>> operator*();

    std::vector<std::shared_ptr<GraphicsModelBase>> currentModels;
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
