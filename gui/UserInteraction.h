//
// Created by petr on 10/11/19.
//

#ifndef TERRAINGENERATION_USERINTERACTION_H
#define TERRAINGENERATION_USERINTERACTION_H

#include "Fwd.h"
#include "UIObject.h"
#include <SDL_events.h>

namespace sdl2cpp::ui {

class MouseInteractable : public virtual UIVisible {
protected:
  friend class EventDispatcher;
  virtual void onMouseDown(const SDL_Event &event) = 0;
  virtual void onMouseUp(const SDL_Event &event) = 0;
  virtual void onMouseMove(const SDL_Event &event) = 0;
  virtual void onMouseClicked(const SDL_Event &event) = 0;
  virtual void onMouseDblClicked(const SDL_Event &event) = 0;
  virtual void onMouseOver(const SDL_Event &event) = 0;
  virtual void onMouseOut(const SDL_Event &event) = 0;
  void onEnabledChanged(bool enabled) override;

  [[nodiscard]] bool isMouseInputEnabled() const;
  void enableMouseInput();
  void disableMouseInput();

private:
  bool mouseInputEnabled = true;
};

class KeyboardInteractable : public virtual UIObject {
protected:
  friend class EventDispatcher;
  virtual void onKeyPressed(const SDL_Event &event) = 0;
  virtual void onKeyDown(const SDL_Event &event) = 0;
  virtual void onKeyUp(const SDL_Event &event) = 0;

  [[nodiscard]] bool isKeyboardInputEnabled() const;
  void enableKeyboardInput();
  void disableKeyboardInput();
 // void onEnabledChanged(bool enabled) override;

private:
  bool keyboardInputEnabled = true;
};
}

#endif // TERRAINGENERATION_USERINTERACTION_H
