//
// Created by petr on 12/6/19.
//

#include "TextArea.h"

sdl2cpp::ui::TextArea::TextArea(UIManager &guiManager, const glm::vec3 &position, const glm::vec3 &dimensions)
    : UIObject(guiManager), UIVisible(position, dimensions) {}

void sdl2cpp::ui::TextArea::onKeyPressed(sdl2cpp::ui::EventInfo info, SDL_Keycode keycode) {
  text = text.get() + SDL_GetKeyName(keycode);
}

void sdl2cpp::ui::TextArea::onKeyDown(sdl2cpp::ui::EventInfo info, [[maybe_unused]] SDL_Keycode keycode) {}

void sdl2cpp::ui::TextArea::onKeyUp(sdl2cpp::ui::EventInfo info, [[maybe_unused]] SDL_Keycode keycode) {}

void sdl2cpp::ui::TextArea::draw([[maybe_unused]] sdl2cpp::ui::GUIRenderer &renderer) {}

void sdl2cpp::ui::TextArea::setText(const std::string &text) { TextArea::text = text; }
void sdl2cpp::ui::TextArea::onFocusChanged([[maybe_unused]] sdl2cpp::ui::Focus focus) {}
std::string sdl2cpp::ui::TextArea::info() const { return "Text area"; }
