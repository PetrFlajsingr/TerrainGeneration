//
// Created by petr on 11/20/19.
//

#include "Interactable.h"
#include "ui/managers/UIManager.h"

bool sdl2cpp::ui::Interactable::areControlsEnabled() const { return controlsEnabled; }

void sdl2cpp::ui::Interactable::enableControls() { controlsEnabled = true; }

void sdl2cpp::ui::Interactable::disableControls() { controlsEnabled = false; }

void sdl2cpp::ui::Interactable::onEnabledChanged(bool enabled) { enabled ? enableControls() : disableControls(); }
void sdl2cpp::ui::Interactable::enableFullControl() {
  getGUIManager().getEventDispatcher().setFullControl(std::dynamic_pointer_cast<Interactable>(shared_from_this()));
}
void sdl2cpp::ui::Interactable::disableFullControl() {
  getGUIManager().getEventDispatcher().disableFullControl();
}
