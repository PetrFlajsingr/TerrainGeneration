//
// Created by petr on 10/11/19.
//

#ifndef TERRAINGENERATION_FOCUSMANAGER_H
#define TERRAINGENERATION_FOCUSMANAGER_H

#include "GUIObject.h"
#include <memory>
#include <vector>
#include "Fwd.h"

namespace sdl2cpp::gui {
class FocusManager {
public:
  explicit FocusManager(std::vector<std::shared_ptr<GUIObject>> &guiObjects) : guiObjects(guiObjects) {

  }

  void changeFocusTo(std::shared_ptr<GUIObject> &guiObject) {
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
  std::weak_ptr<GUIObject> focusedObject;
  std::vector<std::shared_ptr<GUIObject>> &guiObjects;
};
}

#endif // TERRAINGENERATION_FOCUSMANAGER_H
