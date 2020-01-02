//
// Created by petr on 11/19/19.
//

#ifndef TERRAINGENERATION_KEYACTION_H
#define TERRAINGENERATION_KEYACTION_H

#include "ui/interface/KeyboardInteractable.h"
#include <utility>

namespace sdl2cpp::ui {
/**
 * Action called when no other object is focused and selected key is pressed.
 */
class KeyAction : public CustomKeyboardInteractable {
  using Action = std::function<void()>;

public:
  explicit KeyAction(UIManager &guiManager, SDL_Keycode key, Action action);
  std::string info() const override;

protected:
  void onKeyPressed(const SDL_Event &event) override;

private:
  SDL_Keycode key;
  Action action;
};
} // namespace sdl2cpp::ui
#endif // TERRAINGENERATION_KEYACTION_H
