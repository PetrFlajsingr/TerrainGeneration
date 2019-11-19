//
// Created by petr on 11/19/19.
//

#include "KeyboardInteractable.h"
#include <error_handling/exceptions.h>

bool sdl2cpp::ui::CustomKeyboardInteractable::isKeyboardInputEnabled() const {
  return keyboardInputEnabled;
}
void sdl2cpp::ui::CustomKeyboardInteractable::enableKeyboardInput() {
  keyboardInputEnabled = true;
}
void sdl2cpp::ui::CustomKeyboardInteractable::disableKeyboardInput() {
  keyboardInputEnabled = false;
}/*
void sdl2cpp::gui::KeyboardInteractable::onEnabledChanged(bool enabled) {
  enabled ? enableKeyboardInput() : disableKeyboardInput();
}*/
void sdl2cpp::ui::KeyboardInteractable::onKeyPressed(const SDL_Event &event) {}
void sdl2cpp::ui::KeyboardInteractable::onKeyDown(const SDL_Event &event) {}
void sdl2cpp::ui::KeyboardInteractable::onKeyUp(const SDL_Event &event) {}
sdl2cpp::ui::KeyboardInteractable &
sdl2cpp::ui::KeyboardInteractable::setOnKeyUp(
    sdl2cpp::ui::Event::KeyUpFnc onUp) {
  e_keyUp = onUp;
  return *this;
}
sdl2cpp::ui::KeyboardInteractable &
sdl2cpp::ui::KeyboardInteractable::setOnKeyDown(
    sdl2cpp::ui::Event::KeyUpFnc onDown) {
  e_keyDown = onDown;
  return *this;
}
sdl2cpp::ui::KeyboardInteractable &
sdl2cpp::ui::KeyboardInteractable::setOnKeyPressed(
    sdl2cpp::ui::Event::KeyUpFnc onUp) {
  e_keyPressed = onUp;
  return *this;
}
