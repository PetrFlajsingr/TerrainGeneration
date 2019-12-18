//
// Created by petr on 11/19/19.
//

#ifndef TERRAINGENERATION_MOUSEINTERACTABLE_H
#define TERRAINGENERATION_MOUSEINTERACTABLE_H

#include "Interactable.h"
#include "meta/meta.h"
#include "ui/Fwd.h"
#include <SDL_events.h>

namespace sdl2cpp::ui {

/**
 * Interface for custom mouse event behavior.
 */
class CustomMouseInteractable : public virtual Interactable, public virtual UIVisible {
  friend class EventDispatcher;

public:
  /**
   * Allow component to accept mouse events from event dispatch
   */
  void enableMouseControls();
  /**
   * Forbid component to accept mouse events from event dispatch
   */
  void disableMouseControls();
  [[nodiscard]] bool areMouseControlsEnabled();

protected:
  friend class EventDispatcher;
  virtual void onMouseDown(const SDL_Event &event);
  virtual void onMouseUp(const SDL_Event &event);
  virtual void onMouseMove(const SDL_Event &event);
  virtual void onMouseClicked(const SDL_Event &event);
  virtual void onMouseDblClicked(const SDL_Event &event);
  virtual void onMouseOver(const SDL_Event &event);
  virtual void onMouseOut(const SDL_Event &event);
  virtual void onMouseWheel(const SDL_Event &event);

private:
  bool mouseControlsEnabled = true;
};

class CustomEventMouseInteractable : public virtual CustomMouseInteractable {
public:
  MouseButtonState getButtonState(MouseButton button) const;

protected:
  void onMouseDown(const SDL_Event &event) final;
  void onMouseUp(const SDL_Event &event) final;
  void onMouseMove(const SDL_Event &event) final;
  void onMouseClicked(const SDL_Event &event) final;
  void onMouseDblClicked(const SDL_Event &event) final;
  void onMouseOver(const SDL_Event &event) final;
  void onMouseOut(const SDL_Event &event) final;
  void onMouseWheel(const SDL_Event &event) final;

  virtual void onMouseDown(EventInfo, MouseButton, SDL_Point);
  virtual void onMouseUp(EventInfo, MouseButton, SDL_Point);
  virtual void onMouseMove(EventInfo, SDL_Point, SDL_Point);
  virtual void onMouseClicked(EventInfo, MouseButton, SDL_Point);
  virtual void onMouseDblClicked(EventInfo, MouseButton, SDL_Point);
  virtual void onMouseOver(EventInfo);
  virtual void onMouseOut(EventInfo);
  virtual void onMouseWheel(EventInfo, ScrollDirection, int);

private:
  MouseButton buttonFromEvent(const SDL_Event &event) const;
  SDL_Point positionFromEvent(const SDL_Event &event) const;
  std::array<MouseButtonState, MouseButtonCount> buttonStates{MouseButtonState::Released, MouseButtonState::Released,
                                                              MouseButtonState::Released};
};

/**
 * Common mouse interaction interface providing callbacks for events.
 */
class MouseInteractable : public virtual CustomEventMouseInteractable {
public:
  MouseInteractable &setMouseDown(Event::MouseDownFnc onDown);
  MouseInteractable &setMouseUp(Event::MouseUpFnc onUp);
  MouseInteractable &setMouseClicked(Event::MouseClickedFnc onClicked);
  MouseInteractable &setMouseDblClicked(Event::MouseDblClickedFnc onDblClicked);
  MouseInteractable &setMouseOver(Event::MouseOverFnc onOver);
  MouseInteractable &setMouseOut(Event::MouseOutFnc onOut);
  MouseInteractable &setMouseMove(Event::MouseMoveFnc onMove);
  MouseInteractable &setMouseWheel(Event::MouseWheelFnc onWheel);

  template <SimpleInvocable F> MouseInteractable &setMouseDown(F onDown);
  template <SimpleInvocable F> MouseInteractable &setMouseUp(F onUp);
  template <SimpleInvocable F> MouseInteractable &setMouseClicked(F onClicked);
  template <SimpleInvocable F> MouseInteractable &setMouseDblClicked(F onDblClicked);
  template <SimpleInvocable F> MouseInteractable &setMouseOver(F onOver);
  template <SimpleInvocable F> MouseInteractable &setMouseOut(F onOut);
  template <SimpleInvocable F> MouseInteractable &setMouseMove(F onMove);
  template <SimpleInvocable F> MouseInteractable &setMouseWheel(F onWheel);

private:
  std::optional<Event::MouseDownFnc> e_onMouseDown = std::nullopt;
  std::optional<Event::MouseUpFnc> e_OnMouseUp = std::nullopt;
  std::optional<Event::MouseClickedFnc> e_onMouseClicked = std::nullopt;
  std::optional<Event::MouseDblClickedFnc> e_onMouseDblClicked = std::nullopt;
  std::optional<Event::MouseOverFnc> e_onMouseOver = std::nullopt;
  std::optional<Event::MouseOutFnc> e_onMouseOut = std::nullopt;
  std::optional<Event::MouseMoveFnc> e_onMouseMove = std::nullopt;
  std::optional<Event::MouseWheelFnc> e_onMouseWheel = std::nullopt;

protected:
  void onMouseDown(EventInfo info, MouseButton button, SDL_Point point) override;
  void onMouseUp(EventInfo info, MouseButton button, SDL_Point point) override;
  void onMouseMove(EventInfo info, SDL_Point point, SDL_Point sdlPoint) override;
  void onMouseClicked(EventInfo info, MouseButton button, SDL_Point point) override;
  void onMouseDblClicked(EventInfo info, MouseButton button, SDL_Point point) override;
  void onMouseOver(EventInfo info) override;
  void onMouseOut(EventInfo info) override;
  void onMouseWheel(EventInfo info, ScrollDirection direction, int i) override;

private:
  [[nodiscard]] MouseButton buttonFromEvent(const SDL_Event &event) const;
  [[nodiscard]] SDL_Point positionFromEvent(const SDL_Event &event) const;
};

template <SimpleInvocable F> MouseInteractable &MouseInteractable::setMouseDown(F onDown) {
  e_onMouseDown = [onDown](EventInfo, MouseButton, SDL_Point) { onDown(); };
  return *this;
}
template <SimpleInvocable F> MouseInteractable &MouseInteractable::setMouseUp(F onUp) {
  e_OnMouseUp = [onUp](EventInfo, MouseButton, SDL_Point) { onUp(); };
  return *this;
}
template <SimpleInvocable F> MouseInteractable &MouseInteractable::setMouseClicked(F onClicked) {
  e_onMouseClicked = [onClicked](EventInfo, MouseButton, SDL_Point) { onClicked(); };
  return *this;
}
template <SimpleInvocable F> MouseInteractable &MouseInteractable::setMouseDblClicked(F onDblClicked) {
  e_onMouseDblClicked = [onDblClicked](EventInfo, MouseButton, SDL_Point) { onDblClicked(); };
  return *this;
}
template <SimpleInvocable F> MouseInteractable &MouseInteractable::setMouseOver(F onOver) {
  e_onMouseOver = [onOver](EventInfo) { onOver(); };
  return *this;
}
template <SimpleInvocable F> MouseInteractable &MouseInteractable::setMouseOut(F onOut) {
  e_onMouseOut = [onOut](EventInfo) { onOut(); };
  return *this;
}
template <SimpleInvocable F> MouseInteractable &MouseInteractable::setMouseMove(F onMove) {
  e_onMouseMove = [onMove](EventInfo, SDL_Point, SDL_Point) { onMove(); };
  return *this;
}
template <SimpleInvocable F> MouseInteractable &MouseInteractable::setMouseWheel(F onWheel) {
  e_onMouseWheel = [onWheel](EventInfo, ScrollDirection, int) { onWheel(); };
  return *this;
}
} // namespace sdl2cpp::ui
#endif // TERRAINGENERATION_MOUSEINTERACTABLE_H
