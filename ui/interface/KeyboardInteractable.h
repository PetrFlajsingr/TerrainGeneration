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

/**
 * Common keyboard interaction interface providing callbacks for events.
 */
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
} // namespace sdl2cpp::ui

#endif // TERRAINGENERATION_KEYBOARDINTERACTABLE_H
