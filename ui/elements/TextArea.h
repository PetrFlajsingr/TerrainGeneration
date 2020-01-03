//
// Created by petr on 12/6/19.
//

#ifndef TERRAINGENERATION_TEXTAREA_H
#define TERRAINGENERATION_TEXTAREA_H

#include "ui/interface/KeyboardInteractable.h"
#include "ui/interface/UIObject.h"
#include "utils/String.h"
#include <observable/observable.hpp>
#include <string>

namespace sdl2cpp::ui {
class TextArea : public CustomEventKeyboardInteractable, public UIVisible {
  OBSERVABLE_PROPERTIES(TextArea)
public:
  TextArea(UIManager &guiManager, const glm::vec3 &position, const glm::vec3 &dimensions);

  observable_property<std::string> text;

  void setText(const std::string &text);

  std::string info() const override;

protected:
  void onKeyPressed(SDL_Keycode keycode) override;
  void onKeyDown(SDL_Keycode keycode) override;
  void onKeyUp(SDL_Keycode keycode) override;
  void draw(GUIRenderer &renderer) override;
  void onFocusChanged(Focus focus) override;

private:
};

} // namespace sdl2cpp::ui

#endif // TERRAINGENERATION_TEXTAREA_H
