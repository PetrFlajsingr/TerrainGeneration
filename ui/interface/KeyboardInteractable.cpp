//
// Created by petr on 11/19/19.
//

#include "KeyboardInteractable.h"

void sdl2cpp::ui::CustomEventKeyboardInteractable::onKeyPressed(
    const SDL_Event &event) {
  onKeyPressed(EventInfo{*this, Event::Type::KeyPressed}, event.key.keysym.sym);
}
void sdl2cpp::ui::CustomEventKeyboardInteractable::onKeyDown(
    const SDL_Event &event) {
  onKeyDown(EventInfo{*this, Event::Type::KeyPressed}, event.key.keysym.sym);
}
void sdl2cpp::ui::CustomEventKeyboardInteractable::onKeyUp(
    const SDL_Event &event) {
  onKeyUp(EventInfo{*this, Event::Type::KeyPressed}, event.key.keysym.sym);
}

void sdl2cpp::ui::KeyboardInteractable::onKeyPressed(
    sdl2cpp::ui::EventInfo info, SDL_Keycode keycode) {
  if (!e_keyPressed.has_value()) {
    return;
  }
  e_keyPressed.value()(info, keycode);
}
void sdl2cpp::ui::KeyboardInteractable::onKeyDown(sdl2cpp::ui::EventInfo info,
                                                  SDL_Keycode keycode) {
  if (!e_keyDown.has_value()) {
    return;
  }
  e_keyDown.value()(info, keycode);
}
void sdl2cpp::ui::KeyboardInteractable::onKeyUp(sdl2cpp::ui::EventInfo info,
                                                SDL_Keycode keycode) {
  if (!e_keyUp.has_value()) {
    return;
  }
  e_keyUp.value()(info, keycode);
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
    sdl2cpp::ui::Event::KeyUpFnc onPressed) {
  e_keyPressed = onPressed;
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
