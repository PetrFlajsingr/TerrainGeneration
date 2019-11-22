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
  explicit FocusManager(std::vector<std::weak_ptr<UIObject>> &guiObjects)
      : guiObjects(guiObjects) {}

  void changeFocusTo(const std::shared_ptr<UIObject> &guiObject) {
    if (guiObject->focus.get() == Focus::Focused) {
      return;
    }
    for (auto &guiObj : guiObjects) {
      if (guiObj.expired()) {
        continue;
      }
      auto ptr = guiObj.lock();
      if (ptr == guiObject) {
        continue;
      }
      ptr->setFocus(Focus::NotFocused);
    }
    guiObject->setFocus(Focus::Focused);
  }

private:
  std::weak_ptr<UIObject> focusedObject;
  std::vector<std::weak_ptr<UIObject>> &guiObjects;
};
}

#endif // TERRAINGENERATION_FOCUSMANAGER_H
