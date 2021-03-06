//
// Created by petr on 10/11/19.
//

#ifndef TERRAINGENERATION_KEYBOARDINTERACTABLE_H
#define TERRAINGENERATION_KEYBOARDINTERACTABLE_H

#include "Interactable.h"
#include "meta/meta.h"
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
  virtual void onKeyPressed(const SDL_Event &event);
  virtual void onKeyDown(const SDL_Event &event);
  virtual void onKeyUp(const SDL_Event &event);

  bool keyboardControlsEnabled = true;
};

class CustomEventKeyboardInteractable : public CustomKeyboardInteractable {
protected:
  void onKeyPressed(const SDL_Event &event) final;
  void onKeyDown(const SDL_Event &event) final;
  void onKeyUp(const SDL_Event &event) final;

  virtual void onKeyPressed(SDL_Keycode);
  virtual void onKeyDown(SDL_Keycode);
  virtual void onKeyUp(SDL_Keycode);
};

/**
 * Common keyboard interaction interface providing callbacks for events.
 */
class KeyboardInteractable : public virtual CustomEventKeyboardInteractable {
public:
  KeyboardInteractable &setOnKeyUp(Event::KeyUpFnc onUp);
  KeyboardInteractable &setOnKeyDown(Event::KeyUpFnc onDown);
  KeyboardInteractable &setOnKeyPressed(Event::KeyUpFnc onPressed);

  template <SimpleInvocable F> KeyboardInteractable &setOnKeyUp(F onUp);
  template <SimpleInvocable F> KeyboardInteractable &setOnKeyDown(F onDown);
  template <SimpleInvocable F> KeyboardInteractable &setOnKeyPressed(F onPressed);

protected:
  void onKeyPressed(SDL_Keycode keycode) override;
  void onKeyDown(SDL_Keycode keycode) override;
  void onKeyUp(SDL_Keycode keycode) override;

private:
  std::optional<Event::KeyUpFnc> e_keyUp = std::nullopt;
  std::optional<Event::KeyDownFnc> e_keyDown = std::nullopt;
  std::optional<Event::KeyPressedFnc> e_keyPressed = std::nullopt;
};

template <SimpleInvocable F> KeyboardInteractable &KeyboardInteractable::setOnKeyUp(F onUp) {
  e_keyUp = [onUp](EventInfo, SDL_Keycode) { onUp(); };
  return *this;
}
template <SimpleInvocable F> KeyboardInteractable &KeyboardInteractable::setOnKeyDown(F onDown) {
  e_keyDown = [onDown](EventInfo, SDL_Keycode) { onDown(); };
  return *this;
}
template <SimpleInvocable F> KeyboardInteractable &KeyboardInteractable::setOnKeyPressed(F onPressed) {
  e_keyPressed = [onPressed](EventInfo, SDL_Keycode) { onPressed(); };
  return *this;
}
} // namespace sdl2cpp::ui

#endif // TERRAINGENERATION_KEYBOARDINTERACTABLE_H
