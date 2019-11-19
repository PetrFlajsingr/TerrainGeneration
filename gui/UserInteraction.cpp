//
// Created by petr on 11/19/19.
//

#include "UserInteraction.h"

bool sdl2cpp::gui::MouseInteractable::isMouseInputEnabled() const {
  return mouseInputEnabled;
}

void sdl2cpp::gui::MouseInteractable::enableMouseInput() {
  mouseInputEnabled = true;
}
void sdl2cpp::gui::MouseInteractable::disableMouseInput() {
  mouseInputEnabled = false;
}
void sdl2cpp::gui::MouseInteractable::onEnabledChanged(bool enabled) {
  enabled ? enableMouseInput() : disableMouseInput();
}

bool sdl2cpp::gui::KeyboardInteractable::isKeyboardInputEnabled() const {
  return keyboardInputEnabled;
}
void sdl2cpp::gui::KeyboardInteractable::enableKeyboardInput() {
  keyboardInputEnabled = true;
}
void sdl2cpp::gui::KeyboardInteractable::disableKeyboardInput() {
  keyboardInputEnabled = false;
}/*
void sdl2cpp::gui::KeyboardInteractable::onEnabledChanged(bool enabled) {
  enabled ? enableKeyboardInput() : disableKeyboardInput();
}*/
