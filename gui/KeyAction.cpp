//
// Created by petr on 11/19/19.
//

#include "KeyAction.h"

KeyAction::KeyAction(SDL_Keycode key, KeyAction::Action action) : GUIObject(0, 0, 0, 0), key(key), action(std::move(action)) {}
void KeyAction::onKeyPressed(const SDL_Event &event) {}
void KeyAction::onKeyDown(const SDL_Event &event) {
  if (event.key.keysym.sym == key) {
    action();
  }
}
void KeyAction::onKeyUp(const SDL_Event &event) {}
void KeyAction::draw() {}
void KeyAction::onVisibilityChanged(sdl2cpp::gui::Visibility visibility) {}
void KeyAction::onFocusChanged(sdl2cpp::gui::Focus focus) {}
void KeyAction::onEnabledChanged(bool enabled) {}
