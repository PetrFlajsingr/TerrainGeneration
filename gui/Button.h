//
// Created by petr on 10/10/19.
//

#ifndef TERRAINGENERATION_BUTTON_H
#define TERRAINGENERATION_BUTTON_H

#include <utility>
#include <SDL2CPP/Window.h>
#include <SDL.h>

#include "Types.h"
#include "UIObject.h"
#include "UserInteraction.h"
#include <set>

namespace sdl2cpp::gui {
class Button : public MouseInteractable {
public:
  // Initialize the variables
  Button(int x, int y, int w, int h);

protected:
  void onMouseDown(const SDL_Event &event) override;
  void onMouseUp(const SDL_Event &event) override;
  void onMouseMove(const SDL_Event &event) override;
  void onMouseClicked(const SDL_Event &event) override;
  void onMouseDblClicked(const SDL_Event &event) override;
  void onMouseOver(const SDL_Event &event) override;
  void onMouseOut(const SDL_Event &event) override;

protected:
  void draw() override;
  void onVisibilityChanged(Visibility visibility) override;
  void onFocusChanged(Focus focus) override;
  void onEnabledChanged(bool enabled) override;

private:
  SDL_Rect box;
};
}
/*
class TextBox : public KeyboardInteractable, public MouseInteractable {};

class TextArea : public KeyboardInteractable, public MouseInteractable {};

class Menu : public MouseInteractable {};

class RadioButton : public MouseInteractable {};

class CheckBox : public MouseInteractable {};

class Slider : public MouseInteractable {};*/

#endif // TERRAINGENERATION_BUTTON_H
