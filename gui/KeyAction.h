//
// Created by petr on 11/19/19.
//

#ifndef TERRAINGENERATION_KEYACTION_H
#define TERRAINGENERATION_KEYACTION_H

#include <utility>

#include "UserInteraction.h"

class KeyAction : public sdl2cpp::ui::KeyboardInteractable {
  using Action = std::function<void()>;
public:
  explicit KeyAction(SDL_Keycode key, Action action);
protected:
  void onKeyPressed(const SDL_Event &event) override;
  void onKeyDown(const SDL_Event &event) override;
  void onKeyUp(const SDL_Event &event) override;
  void draw() override;
  void onFocusChanged(sdl2cpp::ui::Focus focus) override;
  void onEnabledChanged(bool enabled) override;

private:
  SDL_Keycode key;
  Action action;
};

#endif // TERRAINGENERATION_KEYACTION_H
