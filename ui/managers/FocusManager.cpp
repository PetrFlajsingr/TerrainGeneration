//
// Created by petr on 10/11/19.
//

#include "FocusManager.h"
sdl2cpp::ui::FocusManager::FocusManager(
    std::vector<std::weak_ptr<UIObject>> &guiObjects)
    : guiObjects(guiObjects) {}

void sdl2cpp::ui::FocusManager::changeFocusTo(
    const std::shared_ptr<UIObject> &guiObject) {
  changeFocusTo(guiObject.get());
}

void sdl2cpp::ui::FocusManager::changeFocusTo(
    sdl2cpp::ui::UIObject *guiObject) {
  for (auto &guiObj : guiObjects) {
    if (guiObj.expired()) {
      continue;
    }
    auto ptr = guiObj.lock();
    if (ptr.get() == guiObject) {
      continue;
    }
    ptr->setFocus(Focus::NotFocused);
  }
  guiObject->setFocus(Focus::Focused);
}
