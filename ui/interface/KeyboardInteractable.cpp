//
// Created by petr on 11/19/19.
//

#include "KeyboardInteractable.h"

void sdl2cpp::ui::CustomKeyboardInteractable::onKeyPressed([[maybe_unused]] const SDL_Event &event) {}
void sdl2cpp::ui::CustomKeyboardInteractable::onKeyDown([[maybe_unused]] const SDL_Event &event) {}
void sdl2cpp::ui::CustomKeyboardInteractable::onKeyUp([[maybe_unused]] const SDL_Event &event) {}

void sdl2cpp::ui::CustomEventKeyboardInteractable::onKeyPressed(const SDL_Event &event) { onKeyPressed(event.key.keysym.sym);
}
void sdl2cpp::ui::CustomEventKeyboardInteractable::onKeyDown(const SDL_Event &event) { onKeyDown(event.key.keysym.sym);
}
void sdl2cpp::ui::CustomEventKeyboardInteractable::onKeyUp(const SDL_Event &event) { onKeyUp(event.key.keysym.sym);
}
void sdl2cpp::ui::CustomEventKeyboardInteractable::onKeyPressed(SDL_Keycode) {}
void sdl2cpp::ui::CustomEventKeyboardInteractable::onKeyDown(SDL_Keycode) {}
void sdl2cpp::ui::CustomEventKeyboardInteractable::onKeyUp(SDL_Keycode) {}

void sdl2cpp::ui::KeyboardInteractable::onKeyPressed(SDL_Keycode keycode) {
  if (!e_keyPressed.has_value()) {
    return;
  }
  e_keyPressed.value()(createEventInfo(Event::Type::KeyPressed), keycode);
}
void sdl2cpp::ui::KeyboardInteractable::onKeyDown(SDL_Keycode keycode) {
  if (!e_keyDown.has_value()) {
    return;
  }
  e_keyDown.value()(createEventInfo(Event::Type::KeyDown), keycode);
}
void sdl2cpp::ui::KeyboardInteractable::onKeyUp(SDL_Keycode keycode) {
  if (!e_keyUp.has_value()) {
    return;
  }
  e_keyUp.value()(createEventInfo(Event::Type::KeyUp), keycode);
}

sdl2cpp::ui::KeyboardInteractable &sdl2cpp::ui::KeyboardInteractable::setOnKeyUp(sdl2cpp::ui::Event::KeyUpFnc onUp) {
  e_keyUp = onUp;
  return *this;
}

sdl2cpp::ui::KeyboardInteractable &sdl2cpp::ui::KeyboardInteractable::setOnKeyDown(sdl2cpp::ui::Event::KeyUpFnc onDown) {
  e_keyDown = onDown;
  return *this;
}

sdl2cpp::ui::KeyboardInteractable &sdl2cpp::ui::KeyboardInteractable::setOnKeyPressed(sdl2cpp::ui::Event::KeyUpFnc onPressed) {
  e_keyPressed = onPressed;
  return *this;
}

void sdl2cpp::ui::CustomKeyboardInteractable::enableKeyboardControls() { keyboardControlsEnabled = true; }

void sdl2cpp::ui::CustomKeyboardInteractable::disableKeyboardControls() { keyboardControlsEnabled = false; }

bool sdl2cpp::ui::CustomKeyboardInteractable::areKeyboardControlsEnabled() { return keyboardControlsEnabled; }
