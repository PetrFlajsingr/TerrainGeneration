//
// Created by petr on 10/11/19.
//

#ifndef TERRAINGENERATION_KEYBOARDINTERACTABLE_H
#define TERRAINGENERATION_KEYBOARDINTERACTABLE_H

#include "Interactable.h"
#include "ui/Fwd.h"
#include <SDL_events.h>

namespace sdl2cpp::ui {

/**
 * Interface for custom keyboard event behavior.
 */
class CustomKeyboardInteractable : public virtual Interactable {
public:
  /**
   * Allow component to accept keyboard events from event dispatch
   */
  void enableKeyboardControls();
  /**
   * Forbid component to accept keyboard events from event dispatch
   */
  void disableKeyboardControls();

  [[nodiscard]] bool areKeyboardControlsEnabled();

protected:
  friend class EventDispatcher;
  virtual void onKeyPressed(const SDL_Event &event) = 0;
  virtual void onKeyDown(const SDL_Event &event) = 0;
  virtual void onKeyUp(const SDL_Event &event) = 0;

  bool keyboardControlsEnabled = true;
};

class CustomEventKeyboardInteractable : public CustomKeyboardInteractable {
protected:
  void onKeyPressed(const SDL_Event &event) override;
  void onKeyDown(const SDL_Event &event) override;
  void onKeyUp(const SDL_Event &event) override;

  virtual void onKeyPressed(EventInfo, SDL_Keycode) = 0;
  virtual void onKeyDown(EventInfo, SDL_Keycode) = 0;
  virtual void onKeyUp(EventInfo, SDL_Keycode) = 0;
};

/**
 * Common keyboard interaction interface providing callbacks for events.
 */
class KeyboardInteractable : public virtual CustomEventKeyboardInteractable {
public:
  KeyboardInteractable &setOnKeyUp(Event::KeyUpFnc onUp);
  KeyboardInteractable &setOnKeyDown(Event::KeyUpFnc onDown);
  KeyboardInteractable &setOnKeyPressed(Event::KeyUpFnc onPressed);

  template <SimpleInvocable F>
  KeyboardInteractable &setOnKeyUp(F onUp);
  template <SimpleInvocable F>
  KeyboardInteractable &setOnKeyDown(F onDown);
  template <SimpleInvocable F>
  KeyboardInteractable &setOnKeyPressed(F onPressed);

protected:
  void onKeyPressed(EventInfo info, SDL_Keycode keycode) final;
  void onKeyDown(EventInfo info, SDL_Keycode keycode) final;
  void onKeyUp(EventInfo info, SDL_Keycode keycode) final;

private:
  std::optional<Event::KeyUpFnc> e_keyUp = std::nullopt;
  std::optional<Event::KeyDownFnc> e_keyDown = std::nullopt;
  std::optional<Event::KeyPressedFnc> e_keyPressed = std::nullopt;
};

template <SimpleInvocable F>
KeyboardInteractable &KeyboardInteractable::setOnKeyUp(F onUp) {
  e_keyUp = [onUp] (EventInfo, SDL_Keycode) {onUp();};
  return *this;
}
template <SimpleInvocable F>
KeyboardInteractable &KeyboardInteractable::setOnKeyDown(F onDown) {
  e_keyDown = [onDown] (EventInfo, SDL_Keycode) {onDown();};
  return *this;
}
template <SimpleInvocable F>
KeyboardInteractable &KeyboardInteractable::setOnKeyPressed(F onPressed) {
  e_keyPressed = [onPressed] (EventInfo, SDL_Keycode) {onPressed();};
  return *this;
}
} // namespace sdl2cpp::ui

#endif // TERRAINGENERATION_KEYBOARDINTERACTABLE_H
