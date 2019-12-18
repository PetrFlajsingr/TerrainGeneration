//
// Created by petr on 10/11/19.
//

#ifndef TERRAINGENERATION_FOCUSMANAGER_H
#define TERRAINGENERATION_FOCUSMANAGER_H

#include "ui/Fwd.h"
#include "ui/interface/UIObject.h"
#include <memory>
#include <vector>

namespace sdl2cpp::ui {
class FocusManager {
public:
  explicit FocusManager(std::vector<std::weak_ptr<UIObject>> &guiObjects);

  void changeFocusTo(const std::shared_ptr<UIObject> &guiObject);

  [[nodiscard]] const std::weak_ptr<UIObject> &getFocusedObject() const;

private:
  std::weak_ptr<UIObject> focusedObject;
  std::vector<std::weak_ptr<UIObject>> &guiObjects;
};
} // namespace sdl2cpp::ui

#endif // TERRAINGENERATION_FOCUSMANAGER_H
