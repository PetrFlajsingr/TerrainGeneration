//
// Created by petr on 11/19/19.
//

#include "KeyAction.h"

sdl2cpp::ui::KeyAction::KeyAction(UIManager &guiManager, SDL_Keycode key, KeyAction::Action action)
    : UIObject(guiManager), key(key), action(std::move(action)) {
  setFocus(sdl2cpp::ui::Focus::Focused);
}
void sdl2cpp::ui::KeyAction::onKeyPressed(const SDL_Event &event) {
  if (event.key.keysym.sym == key) {
    action();
  }
}
std::string sdl2cpp::ui::KeyAction::info() const { return "Key action"; }
