//
// Created by petr on 11/20/19.
//

#include "Interactable.h"
bool sdl2cpp::ui::Interactable::areControlsEnabled() const {
  return controlsEnabled;
}
void sdl2cpp::ui::Interactable::enableControls() { controlsEnabled = true; }
void sdl2cpp::ui::Interactable::disableControls() { controlsEnabled = false; }
void sdl2cpp::ui::Interactable::onEnabledChanged(bool enabled) {
  enabled ? enableControls() : disableControls();
}

