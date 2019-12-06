//
// Created by petr on 10/11/19.
//

#include "EventDispatcher.h"
#include "FocusManager.h"
#include "time/now.h"
#include "ui/interface/KeyboardInteractable.h"
#include <chrono>

sdl2cpp::ui::EventDispatcher::EventDispatcher(
    std::shared_ptr<sdl2cpp::Window> window, FocusManager &focusManager)
    : window(std::move(window)), focusManager(focusManager) {
  for (auto event : keyboardEvents) {
    this->window->setEventCallback(event, [this](const auto &event) {
      return keyboardEventHandler(event);
    });
  }
  for (auto event : mouseEvents) {
    this->window->setEventCallback(
        event, [this](const auto &event) { return mouseEventHandler(event); });
  }
}

void sdl2cpp::ui::EventDispatcher::addMouseEventListener(
    const std::weak_ptr<sdl2cpp::ui::CustomMouseInteractable>
        &mouseInteractable) {
  mouseEventListeners.emplace_back(mouseInteractable);
  std::sort(mouseEventListeners.begin(), mouseEventListeners.end(),
            [](std::weak_ptr<CustomMouseInteractable> &a,
               std::weak_ptr<CustomMouseInteractable> &b) {
              if (a.expired() || b.expired()) {
                return false; // FIXME: can loop
              }
              auto lckA = a.lock();
              auto lckB = b.lock();
              return lckA->position.get().z > lckB->position.get().z;
            });
}
void sdl2cpp::ui::EventDispatcher::addKeyboardEventListener(
    const std::weak_ptr<sdl2cpp::ui::CustomKeyboardInteractable>
        &keyboardInteractable) {
  keyboardEventListeners.emplace_back(keyboardInteractable);
}
bool sdl2cpp::ui::EventDispatcher::mouseEventHandler(const SDL_Event &event) {
  using namespace std::chrono_literals;
  using namespace std::chrono;
  static auto lastClickTime = 0ms;
  const auto currentTime = now<std::chrono::milliseconds>();
  int x;
  int y;
  x = event.motion.x;
  y = event.motion.y;

  if (event.type == SDL_MOUSEWHEEL) {
    if (mouseIn != nullptr) {
      mouseIn->onMouseWheel(event);
    }
    return true;
  }

  if (auto el = findMouseInteractibleOnPosition(x, y); el.has_value()) {
    auto &element = el.value();
    if (!element->areControlsEnabled() || !element->areMouseControlsEnabled()) {
      return true;
    }
    switch (event.type) {
    case SDL_MOUSEMOTION:
      if (mouseIn == nullptr) {
        element->onMouseOver(event);
        mouseIn = element.get();
      } else if (mouseIn != element.get()) {
        mouseIn->onMouseOut(event);
        element->onMouseOver(event);
        mouseIn = element.get();
      }
      element->onMouseMove(event);
      break;
    case SDL_MOUSEBUTTONDOWN:
      mouseDownIn = element.get();
      element->onMouseDown(event);
      break;
    case SDL_MOUSEBUTTONUP:
      element->onMouseUp(event);
      if (mouseDownIn == element.get()) {
        if ((currentTime - lastClickTime) < 400ms) {
          element->onMouseDblClicked(event);
          doubleClicked = true;
          lastClickTime = 0ms;
        } else {
          doubleClicked = false;
          addEvent(TimedEvent::SingleShot(
              [element, event, this] {
                if (!doubleClicked) {
                  element->onMouseClicked(event);
                  focusManager.changeFocusTo(element);
                }
              },
              400ms));
          lastClickTime = currentTime;
        }
      }
      mouseDownIn = nullptr;
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
  static auto wasKeyDown = false;
  if (auto el = getFocusedKeyboardInteractible(); el.has_value()) {
    auto &element = el.value();
    if (!element->areControlsEnabled() ||
        !element->areKeyboardControlsEnabled()) {
      return true;
    }
    switch (event.key.type) {
    case SDL_KEYDOWN:
      element->onKeyDown(event);
      wasKeyDown = true;
      break;
    case SDL_KEYUP:
      element->onKeyUp(event);
      if (wasKeyDown) {
        element->onKeyPressed(event);
        wasKeyDown = false;
      }
      break;
    }
    return true;
  }
  return false;
}

bool isIn(SDL_Point point, SDL_Rect rect) {
  return rect.x <= point.x && point.x <= (rect.x + rect.w) &&
         rect.y <= point.y && point.y <= (rect.y + rect.h);
}

std::optional<std::shared_ptr<sdl2cpp::ui::CustomMouseInteractable>>
sdl2cpp::ui::EventDispatcher::findMouseInteractibleOnPosition(int x, int y) {
  for (auto iter = mouseEventListeners.begin();
       iter != mouseEventListeners.end(); ++iter) {
    if (iter->expired()) {
      mouseEventListeners.erase(iter);
    }
    auto ptr = iter->lock();
    SDL_Rect rect{static_cast<int>(ptr->position.get().x),
                  static_cast<int>(ptr->position.get().y),
                  static_cast<int>(ptr->dimensions.get().x),
                  static_cast<int>(ptr->dimensions.get().y)};
    if (isIn(SDL_Point{x, y}, rect)) {
      return ptr;
    }
  }
  return std::nullopt;
}

std::optional<std::shared_ptr<sdl2cpp::ui::CustomKeyboardInteractable>>
sdl2cpp::ui::EventDispatcher::getFocusedKeyboardInteractible() {
  for (auto iter = keyboardEventListeners.begin();
       iter != keyboardEventListeners.end(); ++iter) {
    if (iter->expired()) {
      keyboardEventListeners.erase(iter);
      continue;
    }
    auto ptr = iter->lock();
    if (ptr->focus.get() == Focus::Focused) {
      return ptr;
    }
  }
  return std::nullopt;
}
void sdl2cpp::ui::EventDispatcher::checkTimedEvents(
    std::chrono::milliseconds currentTime) {
  bool anyFired = false;
  for (auto &event : events) {
    if (event.shouldFire(currentTime)) {
      event(currentTime);
      anyFired = true;
    }
  }
  if (anyFired) {
    auto iter = events.begin();
    while (iter != events.end()) {
      if (!iter->isValid()) {
        events.erase(iter++);
      } else {
        ++iter;
      }
    }
  }
}
void sdl2cpp::ui::EventDispatcher::addEvent(sdl2cpp::ui::TimedEvent &&event) {
  events.emplace_back(event);
}
