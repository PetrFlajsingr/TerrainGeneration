//
// Created by petr on 11/19/19.
//

#include "KeyAction.h"

sdl2cpp::ui::KeyAction::KeyAction(UIManager &guiManager, SDL_Keycode key,
                                  KeyAction::Action action)
    : UIObject(guiManager), key(key), action(std::move(action)) {
  setFocus(sdl2cpp::ui::Focus::Focused);
}
void sdl2cpp::ui::KeyAction::onKeyPressed(const SDL_Event &event) {
  if (event.key.keysym.sym == key) {
    action();
  }
}

void sdl2cpp::ui::KeyAction::onKeyDown(const SDL_Event &event) {}

void sdl2cpp::ui::KeyAction::onKeyUp(const SDL_Event &event) {}

void sdl2cpp::ui::KeyAction::onFocusChanged(sdl2cpp::ui::Focus focus) {}

void sdl2cpp::ui::KeyAction::onEnabledChanged(bool enabled) {}
