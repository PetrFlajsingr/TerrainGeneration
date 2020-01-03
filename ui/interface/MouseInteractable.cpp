//
// Created by petr on 11/19/19.
//

#include "MouseInteractable.h"
#include "ui/managers/UIManager.h"
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
  const auto position = positionFromEvent(event);
  onMouseDown(button, position);
}
void sdl2cpp::ui::CustomEventMouseInteractable::onMouseUp(const SDL_Event &event) {
  MouseButton button = buttonFromEvent(event);
  buttonStates[static_cast<int>(button)] = MouseButtonState::Released;
  const auto position = positionFromEvent(event);
  onMouseUp(button, position);
}
void sdl2cpp::ui::CustomEventMouseInteractable::onMouseMove(const SDL_Event &event) {
  const auto newPosition = positionFromEvent(event);
  auto oldPosition = newPosition;
  oldPosition.x -= event.motion.xrel;
  oldPosition.y -= event.motion.yrel;
  onMouseMove(newPosition, oldPosition);
}
void sdl2cpp::ui::CustomEventMouseInteractable::onMouseClicked(const SDL_Event &event) {
  MouseButton button = buttonFromEvent(event);
  const auto position = positionFromEvent(event);
  onMouseClicked(button, position);
}
void sdl2cpp::ui::CustomEventMouseInteractable::onMouseDblClicked(const SDL_Event &event) {
  MouseButton button = buttonFromEvent(event);
  const auto position = positionFromEvent(event);
  onMouseDblClicked(button, position);
}
void sdl2cpp::ui::CustomEventMouseInteractable::onMouseOver([[maybe_unused]] const SDL_Event &event) { onMouseOver(); }
void sdl2cpp::ui::CustomEventMouseInteractable::onMouseOut([[maybe_unused]] const SDL_Event &event) {
  buttonStates[static_cast<int>(MouseButton::Left)] = MouseButtonState::Released;
  buttonStates[static_cast<int>(MouseButton::Middle)] = MouseButtonState::Released;
  buttonStates[static_cast<int>(MouseButton::Right)] = MouseButtonState::Released;
  onMouseOut();
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
  onMouseWheel(direction, delta);
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

glm::vec2 sdl2cpp::ui::CustomEventMouseInteractable::positionFromEvent(const SDL_Event &event) {
  const auto [windowWidth, windowHeight] = getUIManager().getWindowSize();
  return {event.button.x / static_cast<float>(windowWidth), event.button.y / static_cast<float>(windowHeight)};
}

sdl2cpp::ui::MouseButtonState sdl2cpp::ui::CustomEventMouseInteractable::getButtonState(sdl2cpp::ui::MouseButton button) const {
  return buttonStates[static_cast<int>(button)];
}
void sdl2cpp::ui::CustomEventMouseInteractable::onMouseDown([[maybe_unused]] MouseButton button,
                                                            [[maybe_unused]] glm::vec2 position) {}
void sdl2cpp::ui::CustomEventMouseInteractable::onMouseUp([[maybe_unused]] MouseButton button,
                                                          [[maybe_unused]] glm::vec2 position) {}
void sdl2cpp::ui::CustomEventMouseInteractable::onMouseMove([[maybe_unused]] glm::vec2 newPos,
                                                            [[maybe_unused]] glm::vec2 oldPos) {}
void sdl2cpp::ui::CustomEventMouseInteractable::onMouseClicked([[maybe_unused]] MouseButton button,
                                                               [[maybe_unused]] glm::vec2 position) {}
void sdl2cpp::ui::CustomEventMouseInteractable::onMouseDblClicked([[maybe_unused]] MouseButton button,
                                                                  [[maybe_unused]] glm::vec2 position) {}
void sdl2cpp::ui::CustomEventMouseInteractable::onMouseOver() {}
void sdl2cpp::ui::CustomEventMouseInteractable::onMouseOut() {}
void sdl2cpp::ui::CustomEventMouseInteractable::onMouseWheel([[maybe_unused]] ScrollDirection direction,
                                                             [[maybe_unused]] int offset) {}

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

glm::vec2 sdl2cpp::ui::MouseInteractable::positionFromEvent(const SDL_Event &event) const {
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

void sdl2cpp::ui::MouseInteractable::onMouseDown(MouseButton button, glm::vec2 point) {
  if (e_onMouseDown.has_value()) {
    e_onMouseDown.value()(createEventInfo(Event::Type::MouseDown), button, point);
  }
}
void sdl2cpp::ui::MouseInteractable::onMouseUp(MouseButton button, glm::vec2 point) {
  if (e_OnMouseUp.has_value()) {
    e_OnMouseUp.value()(createEventInfo(Event::Type::MouseUp), button, point);
  }
}
void sdl2cpp::ui::MouseInteractable::onMouseMove(glm::vec2 point, glm::vec2 sdlPoint) {
  if (e_onMouseMove.has_value()) {
    e_onMouseMove.value()(createEventInfo(Event::Type::MouseMove), point, sdlPoint);
  }
}
void sdl2cpp::ui::MouseInteractable::onMouseClicked(MouseButton button, glm::vec2 point) {
  if (e_onMouseClicked.has_value()) {
    e_onMouseClicked.value()(createEventInfo(Event::Type::MouseClicked), button, point);
  }
}
void sdl2cpp::ui::MouseInteractable::onMouseDblClicked(MouseButton button, glm::vec2 point) {
  if (e_onMouseDblClicked.has_value()) {
    e_onMouseDblClicked.value()(createEventInfo(Event::Type::MouseDblClicked), button, point);
  }
}
void sdl2cpp::ui::MouseInteractable::onMouseOver() {
  if (e_onMouseOver.has_value()) {
    e_onMouseOver.value()(createEventInfo(Event::Type::MouseOver));
  }
}
void sdl2cpp::ui::MouseInteractable::onMouseOut() {
  if (e_onMouseOut.has_value()) {
    e_onMouseOut.value()(createEventInfo(Event::Type::MouseOut));
  }
}
void sdl2cpp::ui::MouseInteractable::onMouseWheel(ScrollDirection direction, int i) {
  if (e_onMouseWheel.has_value()) {
    e_onMouseWheel.value()(createEventInfo(Event::Type::MouseWheel), direction, i);
  }
}

void sdl2cpp::ui::CustomMouseInteractable::enableMouseControls() { mouseControlsEnabled = true; }

void sdl2cpp::ui::CustomMouseInteractable::disableMouseControls() { mouseControlsEnabled = false; }

bool sdl2cpp::ui::CustomMouseInteractable::areMouseControlsEnabled() { return mouseControlsEnabled; }
