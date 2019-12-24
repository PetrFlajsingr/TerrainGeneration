//
// Created by petr on 11/23/19.
//

#include "Label.h"
#include <ui/managers/UIManager.h>

sdl2cpp::ui::Label::Label(sdl2cpp::ui::UIManager &guiManager, glm::vec3 position, glm::vec3 dimensions)
    : UIObject(guiManager), UIVisible(position, dimensions), text(guiManager.getTextRenderer().createText()) {
  text.setPosition({position.x / 1920 * 1000, 562.5 - (position.y + dimensions.y) / 1080 * 562.5, 0});
  text.setColor({0, 0, 1, 1});
}

void sdl2cpp::ui::Label::draw(sdl2cpp::ui::GUIRenderer &renderer) {}
void sdl2cpp::ui::Label::onVisibilityChanged(sdl2cpp::ui::Visibility visibility) {}
void sdl2cpp::ui::Label::onFocusChanged(sdl2cpp::ui::Focus focus) {}
void sdl2cpp::ui::Label::onEnabledChanged(bool enabled) {}
std::string sdl2cpp::ui::Label::info() const { return "Label"; }
