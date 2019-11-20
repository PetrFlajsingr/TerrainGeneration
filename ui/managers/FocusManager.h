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
  explicit FocusManager(std::vector<std::shared_ptr<UIObject>> &guiObjects) : guiObjects(guiObjects) {

  }

  void changeFocusTo(std::shared_ptr<UIObject> &guiObject) {
    if (guiObject->isFocused()) {
      return;
    }
    for (auto &guiObj : guiObjects) {
      if (guiObj == guiObject) {
        continue;
      }
      guiObj->setFocus(Focus::NotFocused);
    }
    guiObject->setFocus(Focus::Focused);
  }

private:
  std::weak_ptr<UIObject> focusedObject;
  std::vector<std::shared_ptr<UIObject>> &guiObjects;
};
}

#endif // TERRAINGENERATION_FOCUSMANAGER_H
