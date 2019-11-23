//
// Created by petr on 10/11/19.
//

#include "UIObject.h"
#include "ui/managers/GUIManager.h"
#include <glm/vec3.hpp>

sdl2cpp::ui::UIObject::UIObject(sdl2cpp::ui::GUIManager &guiManager)
    : guiManager(guiManager) {}

void sdl2cpp::ui::UIObject::setEnabled(bool enabled) {
  if (this->enabled.get() == enabled) {
    return;
  }
  this->enabled = enabled;
  onEnabledChanged(enabled);
}

void sdl2cpp::ui::UIObject::setFocus(sdl2cpp::ui::Focus focus) {
  if (this->focus.get() == focus) {
    return;
  }
  if (focus == Focus::Focused) {
    this->focus = focus;
    guiManager.getFocusManager().changeFocusTo(this);
  } else {
    this->focus = focus;
  }
  onFocusChanged(focus);
}
sdl2cpp::ui::GUIManager &sdl2cpp::ui::UIObject::getGUIManager() {
  return guiManager;
}

void sdl2cpp::ui::UIVisible::setVisibility(sdl2cpp::ui::Visibility visibility) {
  if (this->visibility.get() == visibility) {
    return;
  }
  this->visibility = visibility;
  onVisibilityChanged(visibility);
}

sdl2cpp::ui::UIVisible::UIVisible(glm::vec3 position, glm::vec3 dimensions)
    : position(position), dimensions(dimensions) {}

void sdl2cpp::ui::UIVisible::setPosition(glm::vec3 position) {
  this->position = position;
}

void sdl2cpp::ui::UIVisible::setDimensions(glm::vec3 dimensions) {
  this->dimensions = dimensions;
}