//
// Created by petr on 10/11/19.
//

#ifndef TERRAINGENERATION_USERINTERACTIONINTERFACES_H
#define TERRAINGENERATION_USERINTERACTIONINTERFACES_H

#include "Fwd.h"
#include "GUIObject.h"

namespace sdl2cpp::gui {

class Interactable : public virtual GUIObject {};

class MouseInteractable : public virtual Interactable {
protected:
  friend class EventDispatcher;
  virtual void onMouseDown(const SDL_Event &event) = 0;
  virtual void onMouseUp(const SDL_Event &event) = 0;
  virtual void onMouseMove(const SDL_Event &event) = 0;
  virtual void onMouseClicked(const SDL_Event &event) = 0;
  virtual void onMouseDblClicked(const SDL_Event &event) = 0;
  virtual void onMouseOver(const SDL_Event &event) = 0;
  virtual void onMouseOut(const SDL_Event &event) = 0;
};

class KeyboardInteractable : public virtual Interactable {
protected:
  friend class EventDispatcher;
  virtual void onKeyPressed(const SDL_Event &event) = 0;
  virtual void onKeyDown(const SDL_Event &event) = 0;
  virtual void onKeyUp(const SDL_Event &event) = 0;
};
}

#endif // TERRAINGENERATION_USERINTERACTIONINTERFACES_H
