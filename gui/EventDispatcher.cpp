//
// Created by petr on 10/11/19.
//

#include "EventDispatcher.h"
#include "UserInteraction.h"

sdl2cpp::ui::EventDispatcher::EventDispatcher(
    std::shared_ptr<sdl2cpp::Window> window)
    : window(std::move(window)){
  for (auto event : keyboardEvents) {
    this->window->setEventCallback(event, [this](const auto &event) {return keyboardEventHandler(event);});
  }
  for (auto event : mouseEvents) {
    this->window->setEventCallback(event, [this](const auto &event) {return mouseEventHandler(event);});
  }
}

void sdl2cpp::ui::EventDispatcher::addMouseEventListener(
    std::weak_ptr<sdl2cpp::ui::MouseInteractable> mouseInteractable) {
  mouseEventListeners.emplace_back(mouseInteractable);
}
void sdl2cpp::ui::EventDispatcher::addKeyboardEventListener(
    std::weak_ptr<sdl2cpp::ui::KeyboardInteractable> keyboardInteractable) {
  keyboardEventListeners.emplace_back(keyboardInteractable);
}
bool sdl2cpp::ui::EventDispatcher::mouseEventHandler(const SDL_Event &event) {
  if (auto element = findMouseInteractibleOnPosition(event.motion.x, event.motion.y); element.has_value()) {
    switch (event.type) {
    case SDL_MOUSEMOTION:
      if (mouseIn == nullptr) {
        element.value()->onMouseOver(event);
        mouseIn = element.value().get();
      } else if (mouseIn != element.value().get()) {
        element.value()->onMouseOut(event);
        mouseIn = element.value().get();
      }
      element.value()->onMouseMove(event);
      break;
    case SDL_MOUSEBUTTONDOWN:
      mouseDownIn = element.value().get();
      element.value()->onMouseDown(event);
      break;
    case SDL_MOUSEBUTTONUP:
      element.value()->onMouseUp(event);
      if (mouseDownIn == element.value().get()) {
        element.value()->onMouseClicked(event);
      }
      mouseDownIn = nullptr;
      break;
    case SDL_MOUSEWHEEL:
      break;
    }
    return true;
  }
  if (event.type == SDL_MOUSEMOTION) {
    if (mouseIn != nullptr) {
      mouseIn->onMouseOut(event);
      mouseIn = nullptr;
    }
    if (mouseDownIn != nullptr) {
      mouseDownIn = nullptr;
    }
  }
  return false;
}
bool sdl2cpp::ui::EventDispatcher::keyboardEventHandler(
    const SDL_Event &event) {
  if (auto element = getFocusedKeyboardInteractible(); element.has_value()) {
    element.value()->onKeyDown(event);
  }
}

bool isIn(SDL_Point point, SDL_Rect rect) {
  return rect.x <= point.x && point.x <= (rect.x + rect.w) && rect.y <= point.y && point.y <= (rect.y + rect.h);
}

std::optional<std::shared_ptr<sdl2cpp::ui::MouseInteractable>>
sdl2cpp::ui::EventDispatcher::findMouseInteractibleOnPosition(int x, int y) {
  for (auto iter = mouseEventListeners.begin(); iter != mouseEventListeners.end(); ++iter) {
    if (iter->expired()) {
      mouseEventListeners.erase(iter);
    }
    auto ptr = iter->lock();
    if (isIn(SDL_Point{x, y}, ptr->getArea()) && ptr->isMouseInputEnabled()) {
      return ptr;
    }
  }
  return std::nullopt;
}
std::optional<std::shared_ptr<sdl2cpp::ui::KeyboardInteractable>>
sdl2cpp::ui::EventDispatcher::getFocusedKeyboardInteractible() {
  for (auto iter = keyboardEventListeners.begin(); iter != keyboardEventListeners.end(); ++iter) {
    if (iter->expired()) {
      keyboardEventListeners.erase(iter);
      return std::nullopt;
    }
    auto ptr = iter->lock();
    if (ptr->isKeyboardInputEnabled()) {
      return ptr;
    }
  }
  return std::nullopt;
}
