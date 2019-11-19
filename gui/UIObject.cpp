//
// Created by petr on 10/11/19.
//

#include "UIObject.h"

void sdl2cpp::ui::UIObject::setEnabled(bool enabled) {
  if (this->enabled == enabled) {
    return;
  }
  this->enabled = enabled;
  onEnabledChanged(enabled);
}

bool sdl2cpp::ui::UIObject::isEnabled() const { return enabled; }

bool sdl2cpp::ui::UIObject::isDisabled() const { return !enabled; }

void sdl2cpp::ui::UIObject::setFocus(sdl2cpp::ui::Focus focus) {
  if (this->focus == focus) {
    return;
  }
  this->focus = focus;
  onFocusChanged(focus);
}

bool sdl2cpp::ui::UIObject::isFocused() const {
  return focus == Focus::Focused;
}

void sdl2cpp::ui::UIVisible::setVisibility(
    sdl2cpp::ui::Visibility visibility) {
  if (this->visiblity == visibility) {
    return;
  }
  this->visiblity = visibility;
  onVisibilityChanged(visiblity);
}


bool sdl2cpp::ui::UIVisible::isVisible() const {
  return visiblity == Visibility::Visible;
}

SDL_Rect sdl2cpp::ui::UIVisible::getArea() const {
  return area;
}
sdl2cpp::ui::UIVisible::UIVisible(SDL_Rect area) : area{area} {}
sdl2cpp::ui::UIVisible::UIVisible(int x, int y, int width, int height) : area{x, y, width, height} {}
