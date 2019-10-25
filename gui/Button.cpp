//
// Created by petr on 10/10/19.
//

#include "Button.h"
#include <SDL_events.h>
#include <iostream>


sdl2cpp::gui::Button::Button(int x, int y, int w, int h) : MouseInteractable(), GUIObject(x, y, w, h)
{
  //Set the button's attributes
  box.x = x;
  box.y = y;
  box.w = w;
  box.h = h;

}
void sdl2cpp::gui::Button::draw() {}
void sdl2cpp::gui::Button::onVisibilityChanged(Visibility visibility) {}
void sdl2cpp::gui::Button::onFocusChanged(Focus focus) {}
void sdl2cpp::gui::Button::onEnabledChanged(bool enabled) {}

void sdl2cpp::gui::Button::onMouseDown(const SDL_Event &event) {
  std::cout << "onMouseDown" << std::endl;
}
void sdl2cpp::gui::Button::onMouseUp(const SDL_Event &event) {
  std::cout << "onMouseUp" << std::endl;
}
void sdl2cpp::gui::Button::onMouseMove(const SDL_Event &event) {
  //std::cout << "onMouseMove" << std::endl;
}
void sdl2cpp::gui::Button::onMouseClicked(const SDL_Event &event) {
  std::cout << "onMouseClicked" << std::endl;
}
void sdl2cpp::gui::Button::onMouseDblClicked(const SDL_Event &event) {
  std::cout << "onMouseDblClicked" << std::endl;
}
void sdl2cpp::gui::Button::onMouseOver(const SDL_Event &event) {
  std::cout << "onMouseOver" << std::endl;
}
void sdl2cpp::gui::Button::onMouseOut(const SDL_Event &event) {
  std::cout << "onMouseOut" << std::endl;
}
/*
bool Button::handle_events(const SDL_Event &event) {
  // The mouse offsets
  int x = 0, y = 0;

  // If the mouse moved
  if (event.type == SDL_MOUSEMOTION) {
    // Get the mouse offsets
    x = event.motion.x;
    y = event.motion.y;

    // If the mouse is over the button
    if ((x > box.x) && (x < box.x + box.w) && (y > box.y) &&
        (y < box.y + box.h)) {
      std::cout << "Mouse over" << std::endl;
    } else {

    }
  }

  //If a mouse button was pressed
  if( event.type == SDL_MOUSEBUTTONDOWN )
  {
    //If the left mouse button was pressed
    if( event.button.button == SDL_BUTTON_LEFT )
    {
      //Get the mouse offsets
      x = event.button.x;
      y = event.button.y;

      //If the mouse is over the button
      if( ( x > box.x ) && ( x < box.x + box.w ) && ( y > box.y ) && ( y < box.y + box.h ) )
      {
        std::cout << "Mouse down" << std::endl;
      }
    }
  }

  //If a mouse button was released
  if( event.type == SDL_MOUSEBUTTONUP )
  {
    //If the left mouse button was released
    if( event.button.button == SDL_BUTTON_LEFT )
    {
      //Get the mouse offsets
      x = event.button.x;
      y = event.button.y;

      //If the mouse is over the button
      if( ( x > box.x ) && ( x < box.x + box.w ) && ( y > box.y ) && ( y < box.y + box.h ) )
      {
        std::cout << "Mouse up" << std::endl;
      }
    }
  }
  return true;
}*/
