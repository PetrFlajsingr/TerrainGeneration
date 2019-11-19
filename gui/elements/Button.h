//
// Created by petr on 10/10/19.
//

#ifndef TERRAINGENERATION_BUTTON_H
#define TERRAINGENERATION_BUTTON_H

#include <utility>
#include <SDL2CPP/Window.h>
#include <SDL.h>

#include "gui/MouseInteractable.h"
#include "gui/UIObject.h"
#include "gui/ui_types.h"
#include <set>

namespace sdl2cpp::ui {

class Button : public MouseInteractable {
public:
  // Initialize the variables
  Button(int x, int y, int w, int h, int zPosition = 0);

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
