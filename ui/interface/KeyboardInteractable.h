//
// Created by petr on 10/11/19.
//

#ifndef TERRAINGENERATION_KEYBOARDINTERACTABLE_H
#define TERRAINGENERATION_KEYBOARDINTERACTABLE_H

#include "UIObject.h"
#include "ui/Fwd.h"
#include <SDL_events.h>

namespace sdl2cpp::ui {

class CustomKeyboardInteractable : public virtual UIObject {
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

class KeyboardInteractable : public virtual CustomKeyboardInteractable {
public:
  KeyboardInteractable &setOnKeyUp(Event::KeyUpFnc onUp);
  KeyboardInteractable &setOnKeyDown(Event::KeyUpFnc onDown);
  KeyboardInteractable &setOnKeyPressed(Event::KeyUpFnc onUp);

protected:
  void onKeyPressed(const SDL_Event &event) override;
  void onKeyDown(const SDL_Event &event) override;
  void onKeyUp(const SDL_Event &event) override;

private:
  std::optional<Event::KeyUpFnc> e_keyUp = std::nullopt;
  std::optional<Event::KeyDownFnc> e_keyDown = std::nullopt;
  std::optional<Event::KeyPressedFnc> e_keyPressed = std::nullopt;
};
}

#endif // TERRAINGENERATION_KEYBOARDINTERACTABLE_H
