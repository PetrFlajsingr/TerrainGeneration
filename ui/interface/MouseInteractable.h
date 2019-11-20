//
// Created by petr on 11/19/19.
//

#ifndef TERRAINGENERATION_MOUSEINTERACTABLE_H
#define TERRAINGENERATION_MOUSEINTERACTABLE_H

#include "UIObject.h"
#include "ui/Fwd.h"
#include <SDL_events.h>

namespace sdl2cpp::ui {

class CustomMouseInteractable : public virtual UIVisible {
  friend class EventDispatcher;

protected:
  friend class EventDispatcher;
  virtual void onMouseDown(const SDL_Event &event) = 0;
  virtual void onMouseUp(const SDL_Event &event) = 0;
  virtual void onMouseMove(const SDL_Event &event) = 0;
  virtual void onMouseClicked(const SDL_Event &event) = 0;
  virtual void onMouseDblClicked(const SDL_Event &event) = 0;
  virtual void onMouseOver(const SDL_Event &event) = 0;
  virtual void onMouseOut(const SDL_Event &event) = 0;

  [[nodiscard]] bool isMouseInputEnabled() const;
  void enableMouseInput();
  void disableMouseInput();

  void onEnabledChanged(bool enabled) override;

private:
  bool mouseInputEnabled = true;
};

class MouseInteractable : public virtual CustomMouseInteractable {
public:
  MouseInteractable &setMouseDown(Event::MouseDownFnc onDown);
  MouseInteractable &setMouseUp(Event::MouseUpFnc onUp);
  MouseInteractable &setMouseClicked(Event::MouseClickedFnc onClicked);
  MouseInteractable &setMouseDblClicked(Event::MouseDblClickedFnc onDblClicked);
  MouseInteractable &setMouseOver(Event::MouseOverFnc onOver);
  MouseInteractable &setMouseOut(Event::MouseOutFnc onOut);
  MouseInteractable &setMouseMove(Event::MouseMoveFnc onMove);

private:
  std::optional<Event::MouseDownFnc> e_onMouseDown = std::nullopt;
  std::optional<Event::MouseUpFnc> e_OnMouseUp = std::nullopt;
  std::optional<Event::MouseClickedFnc> e_onMouseClicked = std::nullopt;
  std::optional<Event::MouseDblClickedFnc> e_onMouseDblClicked = std::nullopt;
  std::optional<Event::MouseOverFnc> e_onMouseOver = std::nullopt;
  std::optional<Event::MouseOutFnc> e_onMouseOut = std::nullopt;
  std::optional<Event::MouseMoveFnc> e_onMouseMove = std::nullopt;

protected:
  void onMouseDown(const SDL_Event &event) override;
  void onMouseUp(const SDL_Event &event) override;
  void onMouseMove(const SDL_Event &event) override;
  void onMouseClicked(const SDL_Event &event) override;
  void onMouseDblClicked(const SDL_Event &event) override;
  void onMouseOver(const SDL_Event &event) override;
  void onMouseOut(const SDL_Event &event) override;

private:
  [[nodiscard]] MouseButton buttonFromEvent(const SDL_Event &event) const;
  [[nodiscard]] SDL_Point positionFromEvent(const SDL_Event &event) const;
};
} // namespace sdl2cpp::ui
#endif // TERRAINGENERATION_MOUSEINTERACTABLE_H
