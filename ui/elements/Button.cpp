//
// Created by petr on 10/10/19.
//

#include "Button.h"
#include <SDL_events.h>
#include <iostream>

sdl2cpp::ui::Button::Button(int x, int y, int w, int h, int zPosition)
    : CustomMouseInteractable(), UIVisible(x, y, w, h, zPosition) {
  box.x = x;
  box.y = y;
  box.w = w;
  box.h = h;

}
void sdl2cpp::ui::Button::draw(GUIRenderer &renderer) {}

void sdl2cpp::ui::Button::onVisibilityChanged(Visibility visibility) {}

void sdl2cpp::ui::Button::onFocusChanged(Focus focus) {}

void sdl2cpp::ui::Button::onEnabledChanged(bool enabled) {}
