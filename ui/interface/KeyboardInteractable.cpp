//
// Created by petr on 11/19/19.
//

#include "KeyboardInteractable.h"
#include <error_handling/exceptions.h>

void sdl2cpp::ui::KeyboardInteractable::onKeyPressed(const SDL_Event &event) {
  if (!e_keyPressed.has_value()) {
    return;
  }
  e_keyPressed.value()(EventInfo{*this, Event::Type::KeyPressed},
                       event.key.keysym.sym);
}
void sdl2cpp::ui::KeyboardInteractable::onKeyDown(const SDL_Event &event) {
  if (!e_keyDown.has_value()) {
    return;
  }
  e_keyDown.value()(EventInfo{*this, Event::Type::KeyDown},
                    event.key.keysym.sym);
}
void sdl2cpp::ui::KeyboardInteractable::onKeyUp(const SDL_Event &event) {
  if (!e_keyUp.has_value()) {
    return;
  }
  e_keyUp.value()(EventInfo{*this, Event::Type::KeyUp}, event.key.keysym.sym);
}
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

void sdl2cpp::ui::CustomKeyboardInteractable::enableKeyboardControls() {
  keyboardControlsEnabled = true;
}
void sdl2cpp::ui::CustomKeyboardInteractable::disableKeyboardControls() {
  keyboardControlsEnabled = false;
}
bool sdl2cpp::ui::CustomKeyboardInteractable::areKeyboardControlsEnabled() {
  return keyboardControlsEnabled;
}
