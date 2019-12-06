//
// Created by petr on 12/6/19.
//

#ifndef TERRAINGENERATION_TEXTAREA_H
#define TERRAINGENERATION_TEXTAREA_H

#include "ui/interface/KeyboardInteractable.h"
#include "ui/interface/UIObject.h"
#include <observable/observable.hpp>
#include "utils/String.h"

namespace sdl2cpp::ui {
class TextArea : public CustomEventKeyboardInteractable, public UIVisible {
  OBSERVABLE_PROPERTIES(TextArea)
public:
  TextArea(UIManager &guiManager, const glm::vec3 &position, const glm::vec3 &dimensions);

  observable_property<std::string> text;

  void setText(const std::string &text);
protected:
  void onKeyPressed(EventInfo info, SDL_Keycode keycode) override;
  void onKeyDown(EventInfo info, SDL_Keycode keycode) override;
  void onKeyUp(EventInfo info, SDL_Keycode keycode) override;
  void draw(GUIRenderer &renderer) override;
  void onFocusChanged(Focus focus) override;
  void onVisibilityChanged(Visibility visibility) override;

private:
};

}

#endif // TERRAINGENERATION_TEXTAREA_H
