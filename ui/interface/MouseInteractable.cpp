//
// Created by petr on 11/19/19.
//

#include "MouseInteractable.h"
#include <error_handling/exceptions.h>

void sdl2cpp::ui::MouseInteractable::onMouseDown(const SDL_Event &event) {
  if (!e_onMouseDown.has_value()) {
    return;
  }
  MouseButton button = buttonFromEvent(event);
  SDL_Point position = positionFromEvent(event);
  e_onMouseDown.value()(EventInfo{*this, Event::Type::MouseDown}, button,
                        position);
}

void sdl2cpp::ui::MouseInteractable::onMouseUp(const SDL_Event &event) {
  if (!e_OnMouseUp.has_value()) {
    return;
  }
  MouseButton button = buttonFromEvent(event);
  SDL_Point position = positionFromEvent(event);
  e_OnMouseUp.value()(EventInfo{*this, Event::Type::MouseUp}, button, position);
}

void sdl2cpp::ui::MouseInteractable::onMouseMove(const SDL_Event &event) {
  if (!e_onMouseMove.has_value()) {
    return;
  }
  SDL_Point newPosition = positionFromEvent(event);
  auto oldPosition = newPosition;
  oldPosition.x -= event.motion.xrel;
  oldPosition.y -= event.motion.yrel;
  e_onMouseMove.value()(EventInfo{*this, Event::Type::MouseMove}, newPosition,
                        oldPosition);
}

void sdl2cpp::ui::MouseInteractable::onMouseDblClicked(const SDL_Event &event) {
  if (!e_onMouseDblClicked.has_value()) {
    return;
  }
  MouseButton button = buttonFromEvent(event);
  SDL_Point position = positionFromEvent(event);
  e_onMouseDblClicked.value()(EventInfo{*this, Event::Type::MouseDblClicked},
                              button, position);
}

void sdl2cpp::ui::MouseInteractable::onMouseOver(const SDL_Event &event) {
  if (!e_onMouseOver.has_value()) {
    return;
  }
  e_onMouseOver.value()(EventInfo{*this, Event::Type::MouseOver});
}

void sdl2cpp::ui::MouseInteractable::onMouseOut(const SDL_Event &event) {
  if (!e_onMouseOut.has_value()) {
    return;
  }
  e_onMouseOut.value()(EventInfo{*this, Event::Type::MouseOut});
}

void sdl2cpp::ui::MouseInteractable::onMouseClicked(const SDL_Event &event) {
  if (!e_onMouseClicked.has_value()) {
    return;
  }
  MouseButton button = buttonFromEvent(event);
  SDL_Point position = positionFromEvent(event);
  e_onMouseClicked.value()(EventInfo{*this, Event::Type::MouseClicked}, button,
                           position);
}

sdl2cpp::ui::MouseButton
sdl2cpp::ui::MouseInteractable::buttonFromEvent(const SDL_Event &event) const {
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

sdl2cpp::ui::MouseInteractable &sdl2cpp::ui::MouseInteractable::setMouseClicked(
    sdl2cpp::ui::Event::MouseClickedFnc onClick) {
  e_onMouseClicked = std::move(onClick);
  return *this;
}

SDL_Point sdl2cpp::ui::MouseInteractable::positionFromEvent(
    const SDL_Event &event) const {
  return {event.button.x, event.button.y};
}

sdl2cpp::ui::MouseInteractable &sdl2cpp::ui::MouseInteractable::setMouseDown(
    sdl2cpp::ui::Event::MouseDownFnc onDown) {
  e_onMouseDown = std::move(onDown);
  return *this;
}

sdl2cpp::ui::MouseInteractable &sdl2cpp::ui::MouseInteractable::setMouseUp(
    sdl2cpp::ui::Event::MouseUpFnc onUp) {
  e_OnMouseUp = std::move(onUp);
  return *this;
}

sdl2cpp::ui::MouseInteractable &
sdl2cpp::ui::MouseInteractable::setMouseDblClicked(
    sdl2cpp::ui::Event::MouseDblClickedFnc onDblClicked) {
  e_onMouseDblClicked = std::move(onDblClicked);
  return *this;
}

sdl2cpp::ui::MouseInteractable &sdl2cpp::ui::MouseInteractable::setMouseOver(
    sdl2cpp::ui::Event::MouseOverFnc onOver) {
  e_onMouseOver = std::move(onOver);
  return *this;
}

sdl2cpp::ui::MouseInteractable &sdl2cpp::ui::MouseInteractable::setMouseOut(
    sdl2cpp::ui::Event::MouseOutFnc onOut) {
  e_onMouseOut = std::move(onOut);
  return *this;
}

sdl2cpp::ui::MouseInteractable &sdl2cpp::ui::MouseInteractable::setMouseMove(
    sdl2cpp::ui::Event::MouseMoveFnc onMove) {
  e_onMouseMove = std::move(onMove);
  return *this;
}

void sdl2cpp::ui::CustomMouseInteractable::enableMouseControls() {
  mouseControlsEnabled = true;
}

void sdl2cpp::ui::CustomMouseInteractable::disableMouseControls() {
  mouseControlsEnabled = false;
}

bool sdl2cpp::ui::CustomMouseInteractable::areMouseControlsEnabled() {
  return mouseControlsEnabled;
}