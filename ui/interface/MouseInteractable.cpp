//
// Created by petr on 11/19/19.
//

#include "MouseInteractable.h"
#include <error_handling/exceptions.h>

void sdl2cpp::ui::CustomMouseInteractable::onMouseDown([[maybe_unused]] const SDL_Event &event) {}
void sdl2cpp::ui::CustomMouseInteractable::onMouseUp([[maybe_unused]] const SDL_Event &event) {}
void sdl2cpp::ui::CustomMouseInteractable::onMouseMove([[maybe_unused]] const SDL_Event &event) {}
void sdl2cpp::ui::CustomMouseInteractable::onMouseClicked([[maybe_unused]] const SDL_Event &event) {}
void sdl2cpp::ui::CustomMouseInteractable::onMouseDblClicked([[maybe_unused]] const SDL_Event &event) {}
void sdl2cpp::ui::CustomMouseInteractable::onMouseOver([[maybe_unused]] const SDL_Event &event) {}
void sdl2cpp::ui::CustomMouseInteractable::onMouseOut([[maybe_unused]] const SDL_Event &event) {}
void sdl2cpp::ui::CustomMouseInteractable::onMouseWheel([[maybe_unused]] const SDL_Event &event) {}

void sdl2cpp::ui::CustomEventMouseInteractable::onMouseDown(const SDL_Event &event) {
  MouseButton button = buttonFromEvent(event);
  buttonStates[static_cast<int>(button)] = MouseButtonState::Pressed;
  SDL_Point position = positionFromEvent(event);
  onMouseDown(EventInfo{*this, Event::Type::MouseDown}, button, position);
}
void sdl2cpp::ui::CustomEventMouseInteractable::onMouseUp(const SDL_Event &event) {
  MouseButton button = buttonFromEvent(event);
  buttonStates[static_cast<int>(button)] = MouseButtonState::Released;
  SDL_Point position = positionFromEvent(event);
  onMouseUp(EventInfo{*this, Event::Type::MouseUp}, button, position);
}
void sdl2cpp::ui::CustomEventMouseInteractable::onMouseMove(const SDL_Event &event) {
  SDL_Point newPosition = positionFromEvent(event);
  auto oldPosition = newPosition;
  oldPosition.x -= event.motion.xrel;
  oldPosition.y -= event.motion.yrel;
  onMouseMove(EventInfo{*this, Event::Type::MouseMove}, newPosition, oldPosition);
}
void sdl2cpp::ui::CustomEventMouseInteractable::onMouseClicked(const SDL_Event &event) {
  MouseButton button = buttonFromEvent(event);
  SDL_Point position = positionFromEvent(event);
  onMouseClicked(EventInfo{*this, Event::Type::MouseClicked}, button, position);
}
void sdl2cpp::ui::CustomEventMouseInteractable::onMouseDblClicked(const SDL_Event &event) {
  MouseButton button = buttonFromEvent(event);
  SDL_Point position = positionFromEvent(event);
  onMouseDblClicked(EventInfo{*this, Event::Type::MouseDblClicked}, button, position);
}
void sdl2cpp::ui::CustomEventMouseInteractable::onMouseOver([[maybe_unused]] const SDL_Event &event) {
  onMouseOver(EventInfo{*this, Event::Type::MouseOver});
}
void sdl2cpp::ui::CustomEventMouseInteractable::onMouseOut([[maybe_unused]] const SDL_Event &event) {
  buttonStates[static_cast<int>(MouseButton::Left)] = MouseButtonState::Released;
  buttonStates[static_cast<int>(MouseButton::Middle)] = MouseButtonState::Released;
  buttonStates[static_cast<int>(MouseButton::Right)] = MouseButtonState::Released;
  onMouseOut(EventInfo{*this, Event::Type::MouseOut});
}
void sdl2cpp::ui::CustomEventMouseInteractable::onMouseWheel(const SDL_Event &event) {
  ScrollDirection direction;
  int delta;
  if (event.wheel.y > 0) {
    direction = ScrollDirection::Up;
    delta = event.wheel.y;
  } else if (event.wheel.y < 0) {
    direction = ScrollDirection::Down;
    delta = event.wheel.y;
  } else if (event.wheel.x > 0) {
    direction = ScrollDirection::Right;
    delta = event.wheel.x;
  } else if (event.wheel.x < 0) {
    direction = ScrollDirection::Left;
    delta = event.wheel.x;
  } else {
    throw exc::InternalError("onMouseWheel: Invalid state.");
  }
  onMouseWheel(EventInfo{*this, Event::Type::MouseWheel}, direction, delta);
}

sdl2cpp::ui::MouseButton sdl2cpp::ui::CustomEventMouseInteractable::buttonFromEvent(const SDL_Event &event) const {
  switch (event.button.button) {
  case SDL_BUTTON_LEFT:
    return MouseButton::Left;
  case SDL_BUTTON_MIDDLE:
    return MouseButton::Middle;
  case SDL_BUTTON_RIGHT:
    return MouseButton::Right;
  }
  throw exc::InternalError("SDL_Event carries non-sensical value.");
}

SDL_Point sdl2cpp::ui::CustomEventMouseInteractable::positionFromEvent(const SDL_Event &event) const {
  return {event.button.x, event.button.y};
}

sdl2cpp::ui::MouseButtonState sdl2cpp::ui::CustomEventMouseInteractable::getButtonState(sdl2cpp::ui::MouseButton button) const {
  return buttonStates[static_cast<int>(button)];
}
void sdl2cpp::ui::CustomEventMouseInteractable::onMouseDown(sdl2cpp::ui::EventInfo, sdl2cpp::ui::MouseButton, SDL_Point) {}
void sdl2cpp::ui::CustomEventMouseInteractable::onMouseUp(sdl2cpp::ui::EventInfo, sdl2cpp::ui::MouseButton, SDL_Point) {}
void sdl2cpp::ui::CustomEventMouseInteractable::onMouseMove(sdl2cpp::ui::EventInfo, SDL_Point, SDL_Point) {}
void sdl2cpp::ui::CustomEventMouseInteractable::onMouseClicked(sdl2cpp::ui::EventInfo, sdl2cpp::ui::MouseButton, SDL_Point) {}
void sdl2cpp::ui::CustomEventMouseInteractable::onMouseDblClicked(sdl2cpp::ui::EventInfo, sdl2cpp::ui::MouseButton, SDL_Point) {}
void sdl2cpp::ui::CustomEventMouseInteractable::onMouseOver(sdl2cpp::ui::EventInfo) {}
void sdl2cpp::ui::CustomEventMouseInteractable::onMouseOut(sdl2cpp::ui::EventInfo) {}
void sdl2cpp::ui::CustomEventMouseInteractable::onMouseWheel(sdl2cpp::ui::EventInfo, sdl2cpp::ui::ScrollDirection, int) {}

sdl2cpp::ui::MouseButton sdl2cpp::ui::MouseInteractable::buttonFromEvent(const SDL_Event &event) const {
  switch (event.button.button) {
  case SDL_BUTTON_LEFT:
    return MouseButton::Left;
  case SDL_BUTTON_MIDDLE:
    return MouseButton::Middle;
  case SDL_BUTTON_RIGHT:
    return MouseButton::Right;
  }
  throw exc::InternalError("SDL_Event carries non-sensical value.");
}

sdl2cpp::ui::MouseInteractable &sdl2cpp::ui::MouseInteractable::setMouseClicked(sdl2cpp::ui::Event::MouseClickedFnc onClick) {
  e_onMouseClicked = std::move(onClick);
  return *this;
}

SDL_Point sdl2cpp::ui::MouseInteractable::positionFromEvent(const SDL_Event &event) const {
  return {event.button.x, event.button.y};
}

sdl2cpp::ui::MouseInteractable &sdl2cpp::ui::MouseInteractable::setMouseDown(sdl2cpp::ui::Event::MouseDownFnc onDown) {
  e_onMouseDown = std::move(onDown);
  return *this;
}

sdl2cpp::ui::MouseInteractable &sdl2cpp::ui::MouseInteractable::setMouseUp(sdl2cpp::ui::Event::MouseUpFnc onUp) {
  e_OnMouseUp = std::move(onUp);
  return *this;
}

sdl2cpp::ui::MouseInteractable &
sdl2cpp::ui::MouseInteractable::setMouseDblClicked(sdl2cpp::ui::Event::MouseDblClickedFnc onDblClicked) {
  e_onMouseDblClicked = std::move(onDblClicked);
  return *this;
}

sdl2cpp::ui::MouseInteractable &sdl2cpp::ui::MouseInteractable::setMouseOver(sdl2cpp::ui::Event::MouseOverFnc onOver) {
  e_onMouseOver = std::move(onOver);
  return *this;
}

sdl2cpp::ui::MouseInteractable &sdl2cpp::ui::MouseInteractable::setMouseOut(sdl2cpp::ui::Event::MouseOutFnc onOut) {
  e_onMouseOut = std::move(onOut);
  return *this;
}

sdl2cpp::ui::MouseInteractable &sdl2cpp::ui::MouseInteractable::setMouseMove(sdl2cpp::ui::Event::MouseMoveFnc onMove) {
  e_onMouseMove = std::move(onMove);
  return *this;
}
sdl2cpp::ui::MouseInteractable &sdl2cpp::ui::MouseInteractable::setMouseWheel(sdl2cpp::ui::Event::MouseWheelFnc onWheel) {
  e_onMouseWheel = onWheel;
  return *this;
}

void sdl2cpp::ui::MouseInteractable::onMouseDown(sdl2cpp::ui::EventInfo info, sdl2cpp::ui::MouseButton button, SDL_Point point) {
  if (e_onMouseDown.has_value()) {
    e_onMouseDown.value()(info, button, point);
  }
}
void sdl2cpp::ui::MouseInteractable::onMouseUp(sdl2cpp::ui::EventInfo info, sdl2cpp::ui::MouseButton button, SDL_Point point) {
  if (e_OnMouseUp.has_value()) {
    e_OnMouseUp.value()(info, button, point);
  }
}
void sdl2cpp::ui::MouseInteractable::onMouseMove(sdl2cpp::ui::EventInfo info, SDL_Point point, SDL_Point sdlPoint) {
  if (e_onMouseMove.has_value()) {
    e_onMouseMove.value()(info, point, sdlPoint);
  }
}
void sdl2cpp::ui::MouseInteractable::onMouseClicked(sdl2cpp::ui::EventInfo info, sdl2cpp::ui::MouseButton button,
                                                    SDL_Point point) {
  if (e_onMouseClicked.has_value()) {
    e_onMouseClicked.value()(info, button, point);
  }
}
void sdl2cpp::ui::MouseInteractable::onMouseDblClicked(sdl2cpp::ui::EventInfo info, sdl2cpp::ui::MouseButton button,
                                                       SDL_Point point) {
  if (e_onMouseDblClicked.has_value()) {
    e_onMouseDblClicked.value()(info, button, point);
  }
}
void sdl2cpp::ui::MouseInteractable::onMouseOver(sdl2cpp::ui::EventInfo info) {
  if (e_onMouseOver.has_value()) {
    e_onMouseOver.value()(info);
  }
}
void sdl2cpp::ui::MouseInteractable::onMouseOut(sdl2cpp::ui::EventInfo info) {
  if (e_onMouseOut.has_value()) {
    e_onMouseOut.value()(info);
  }
}
void sdl2cpp::ui::MouseInteractable::onMouseWheel(sdl2cpp::ui::EventInfo info, sdl2cpp::ui::ScrollDirection direction, int i) {
  if (e_onMouseWheel.has_value()) {
    e_onMouseWheel.value()(info, direction, i);
  }
}

void sdl2cpp::ui::CustomMouseInteractable::enableMouseControls() { mouseControlsEnabled = true; }

void sdl2cpp::ui::CustomMouseInteractable::disableMouseControls() { mouseControlsEnabled = false; }

bool sdl2cpp::ui::CustomMouseInteractable::areMouseControlsEnabled() { return mouseControlsEnabled; }
