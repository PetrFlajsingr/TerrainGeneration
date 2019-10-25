//
// Created by petr on 10/11/19.
//

#include "GUIObject.h"

sdl2cpp::gui::GUIObject::GUIObject(SDL_Rect area) : area(area) {}

sdl2cpp::gui::GUIObject::GUIObject(int x, int y, int width, int height)
    : area(SDL_Rect{x, y, width, height}) {}

void sdl2cpp::gui::GUIObject::setVisibility(
    sdl2cpp::gui::Visibility visibility) {
  if (this->visiblity == visibility) {
    return;
  }
  this->visiblity = visibility;
  onVisibilityChanged(visiblity);
}

void sdl2cpp::gui::GUIObject::setEnabled(bool enabled) {
  if (this->enabled == enabled) {
    return;
  }
  this->enabled = enabled;
  onEnabledChanged(enabled);
}

bool sdl2cpp::gui::GUIObject::isVisible() const {
  return visiblity == Visibility::Visible;
}

bool sdl2cpp::gui::GUIObject::isEnabled() const { return enabled; }

bool sdl2cpp::gui::GUIObject::isDisabled() const { return !enabled; }

void sdl2cpp::gui::GUIObject::setFocus(sdl2cpp::gui::Focus focus) {
  if (this->focus == focus) {
    return;
  }
  this->focus = focus;
  onFocusChanged(focus);
}

SDL_Rect sdl2cpp::gui::GUIObject::getArea() const {
  return area;
}
bool sdl2cpp::gui::GUIObject::isFocused() const {
  return focus == Focus::Focused;
}
