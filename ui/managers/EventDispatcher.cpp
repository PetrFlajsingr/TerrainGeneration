//
// Created by petr on 10/11/19.
//

#include "EventDispatcher.h"
#include "FocusManager.h"
#include "UIManager.h"
#include <graphics/BoundingBox.h>
#include <io/print.h>

sdl2cpp::ui::EventDispatcher::EventDispatcher(UIManager &uiManager, FocusManager &focusManager)
    : uiManager(uiManager), focusManager(focusManager) {
  registerWindowEvents();
}

void sdl2cpp::ui::EventDispatcher::registerWindowEvents() {
  for (auto event : keyboardEvents) {
    uiManager.getWindow().setEventCallback(event, [this](const auto &event) { return keyboardEventHandler(event); });
  }
  for (auto event : mouseEvents) {
    uiManager.getWindow().setEventCallback(event, [this](const auto &event) { return mouseEventHandler(event); });
  }
}

void sdl2cpp::ui::EventDispatcher::addMouseEventListener(
    const std::weak_ptr<sdl2cpp::ui::CustomMouseInteractable> &mouseInteractable) {
  mouseEventListeners.emplace_back(mouseInteractable);
  std::sort(mouseEventListeners.begin(), mouseEventListeners.end(),
            [](std::weak_ptr<CustomMouseInteractable> &a, std::weak_ptr<CustomMouseInteractable> &b) {
              if (a.expired() || b.expired()) {
                return false; // FIXME: can loop
              }
              auto lckA = a.lock();
              auto lckB = b.lock();
              return lckA->position.get().z > lckB->position.get().z;
            });
}

void sdl2cpp::ui::EventDispatcher::addKeyboardEventListener(
    const std::weak_ptr<sdl2cpp::ui::CustomKeyboardInteractable> &keyboardInteractable) {
  keyboardEventListeners.emplace_back(keyboardInteractable);
}

bool sdl2cpp::ui::EventDispatcher::mouseEventHandler(const SDL_Event &event) {
  using namespace std::chrono_literals;
  using namespace std::chrono;
  static auto lastClickTime = 0ms;
  const auto currentTime = now<std::chrono::milliseconds>();

  const auto [windowWidth, windowHeight] = uiManager.getWindowSize();
  const auto x = event.motion.x / static_cast<float>(windowWidth);
  const auto y = event.motion.y / static_cast<float>(windowHeight);

  if (event.type == SDL_MOUSEWHEEL) {
    if (mouseIn != nullptr) {
      mouseIn->onMouseWheel(event);
    }
    return true;
  }

  if (auto el = findMouseInteractableOnPosition(x, y); el.has_value()) {
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
        if ((currentTime - lastClickTime) < 200ms) {
          if (clickEvent != nullptr) {
            clickEvent->invalidate();
            clickEvent = nullptr;
          }
          element->onMouseDblClicked(event);
          lastClickTime = 0ms;
        } else {
          clickEvent = &addEvent(TimedEvent::SingleShot(
              [element, event, this] {
                element->onMouseClicked(event);
                focusManager.changeFocusTo(element);
              },
              200ms));
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
bool sdl2cpp::ui::EventDispatcher::keyboardEventHandler(const SDL_Event &event) {
  static auto wasKeyDown = false;
  if (auto el = getFocusedKeyboardInteractable(); el.has_value()) {
    auto &element = el.value();
    if (!element->areControlsEnabled() || !element->areKeyboardControlsEnabled()) {
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
  return rect.x <= point.x && point.x <= (rect.x + rect.w) && rect.y <= point.y && point.y <= (rect.y + rect.h);
}

std::optional<std::shared_ptr<sdl2cpp::ui::CustomMouseInteractable>>
sdl2cpp::ui::EventDispatcher::findMouseInteractableOnPosition(float x, float y) {
  for (auto iter = mouseEventListeners.begin(); iter != mouseEventListeners.end(); ++iter) {
    if (iter->expired()) {
      mouseEventListeners.erase(iter);
    }
    auto ptr = iter->lock();
    const glm::vec2 p1{ptr->position.get().x, ptr->position.get().y};
    const glm::vec2 p2 = p1 + glm::vec2{ptr->dimensions.get().x, ptr->dimensions.get().y};
    geo::BoundingBox<2> bb{p1, p2};
    if (bb.contains({x, y})) {
      if (fullControl.has_value() && !fullControl->expired()) {
        auto locked = fullControl->lock();
        if (ptr != locked) {
          continue;
        }
      }
      return ptr;
    }
  }
  return std::nullopt;
}

std::optional<std::shared_ptr<sdl2cpp::ui::CustomKeyboardInteractable>>
sdl2cpp::ui::EventDispatcher::getFocusedKeyboardInteractable() {
  if (focusManager.getFocusedObject().expired()) {
    return std::nullopt;
  }
  auto sharedFocusedObject = focusManager.getFocusedObject().lock();
  if (auto focusedKeyboardInteractable = std::dynamic_pointer_cast<CustomKeyboardInteractable>(sharedFocusedObject);
      focusedKeyboardInteractable != nullptr) {
    return focusedKeyboardInteractable;
  }
  return std::nullopt;
}

void sdl2cpp::ui::EventDispatcher::checkTimedEvents(std::chrono::milliseconds currentTime) {
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

sdl2cpp::ui::TimedEvent &sdl2cpp::ui::EventDispatcher::addEvent(sdl2cpp::ui::TimedEvent &&event) {
  events.emplace_back(event);
  return events.back();
}
void sdl2cpp::ui::EventDispatcher::setFullControl(std::weak_ptr<Interactable> element) { fullControl = element; }
void sdl2cpp::ui::EventDispatcher::disableFullControl() { fullControl = std::nullopt; }
