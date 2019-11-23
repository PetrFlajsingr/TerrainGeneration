//
// Created by petr on 11/23/19.
//

#include "Label.h"
#include <ui/managers/GUIManager.h>
#include <ui/utils.h>

sdl2cpp::ui::Label::Label(sdl2cpp::ui::GUIManager &guiManager,
                          glm::vec3 position, glm::vec3 dimensions)
    : UIObject(guiManager), MouseInteractable(),
      UIVisible(position, dimensions), text(guiManager.getFontManager()) {
  SDL_Rect rect{static_cast<int>(position.x), static_cast<int>(position.y),
                static_cast<int>(dimensions.x), static_cast<int>(dimensions.y)};
  auto positions = sdlRectToGLCoordinates(rect, 1920, 1080);
  text.setPosition({position.x / 1920 * 1000,
                    562.5 - (position.y + dimensions.y) / 1080 * 562.5, 0});
  text.setColor({0, 0, 1, 1});
}

void sdl2cpp::ui::Label::onFocusChanged(sdl2cpp::ui::Focus focus) {}
void sdl2cpp::ui::Label::onEnabledChanged(bool enabled) {}
void sdl2cpp::ui::Label::draw(sdl2cpp::ui::GUIRenderer &renderer) {}
void sdl2cpp::ui::Label::onVisibilityChanged(
    sdl2cpp::ui::Visibility visibility) {}
