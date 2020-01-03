//
// Created by petr on 11/23/19.
//

#include "Label.h"
#include <ui/managers/UIManager.h>

sdl2cpp::ui::Label::Label(sdl2cpp::ui::UIManager &guiManager, glm::vec3 position, glm::vec3 dimensions)
    : UIObject(guiManager), UIVisible(position, dimensions), text(guiManager.getTextRenderer().createText()) {
  text.setPosition({position.x * 1000, 562.5 - (position.y + dimensions.y) * 562.5, 0});
  text.setColor({0, 0, 1, 1});
}

void sdl2cpp::ui::Label::draw([[maybe_unused]] sdl2cpp::ui::GUIRenderer &renderer) {}
std::string sdl2cpp::ui::Label::info() const { return "Label"; }
