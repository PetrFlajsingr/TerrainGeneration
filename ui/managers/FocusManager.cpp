//
// Created by petr on 10/11/19.
//

#include "FocusManager.h"

sdl2cpp::ui::FocusManager::FocusManager(std::vector<std::weak_ptr<UIObject>> &guiObjects) : guiObjects(guiObjects) {}

void sdl2cpp::ui::FocusManager::changeFocusTo(const std::shared_ptr<UIObject> &guiObject) {
  std::shared_ptr<UIObject> currentlyFocused = nullptr;
  if (!focusedObject.expired()) {
    currentlyFocused = focusedObject.lock();
  }
  if (currentlyFocused == guiObject) {
    return;
  }
  if (currentlyFocused != nullptr) {
    currentlyFocused->setFocus(Focus::NotFocused);
  }
  guiObject->setFocus(Focus::Focused);
  focusedObject = guiObject;
}

const std::weak_ptr<sdl2cpp::ui::UIObject> &sdl2cpp::ui::FocusManager::getFocusedObject() const { return focusedObject; }
