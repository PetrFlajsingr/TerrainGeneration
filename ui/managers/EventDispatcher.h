//
// Created by petr on 10/11/19.
//

#ifndef TERRAINGENERATION_EVENTDISPATCHER_H
#define TERRAINGENERATION_EVENTDISPATCHER_H

#include "ui/Fwd.h"
#include "ui/TimedEvent.h"
#include "ui/interface/KeyboardInteractable.h"
#include "ui/interface/MouseInteractable.h"
#include <SDL2CPP/Window.h>
#include <list>
#include <memory>
#include <meta/meta.h>
#include <set>
#include <vector>

namespace sdl2cpp::ui {

using namespace std::chrono_literals;

class EventDispatcher {
  friend class UIManager;

public:
  explicit EventDispatcher(std::shared_ptr<Window> window, FocusManager &focusManager);

  /**
   * Add event to timed event list.
   * @param event event to add - EventDispatcher moves it
   * @return reference to added event
   */
  TimedEvent &addEvent(TimedEvent &&event);
  /**
   * Check if any of TimedEvents should be fired or removed.
   * @param currentTime current timestamp
   */
  void checkTimedEvents(std::chrono::milliseconds currentTime);

  void addMouseEventListener(const std::weak_ptr<CustomMouseInteractable> &mouseInteractable);
  void addKeyboardEventListener(const std::weak_ptr<CustomKeyboardInteractable> &keyboardInteractable);

private:
  std::list<TimedEvent> events;
  TimedEvent *clickEvent = nullptr;

  std::shared_ptr<Window> window;
  FocusManager &focusManager;

  bool mouseEventHandler(const SDL_Event &event);
  bool keyboardEventHandler(const SDL_Event &event);

  // element in which mouse down event occurred last
  CustomMouseInteractable *mouseDownIn = nullptr;
  // element into which mouse has moved last
  CustomMouseInteractable *mouseIn = nullptr;

  /**
   * Find mouse interactable element in given coordinates.
   * @param x
   * @param y
   * @return mouse interactable on position or std::nullopt if none was found on
   * given position
   */
  std::optional<std::shared_ptr<CustomMouseInteractable>> findMouseInteractableOnPosition(int x, int y);
  /**
   * Find keyboard interactable currently in focus.
   * @return focused keyobard interactable or std::nullopt if no element has
   * focus
   */
  std::optional<std::shared_ptr<CustomKeyboardInteractable>> getFocusedKeyboardInteractable();

  void registerWindowEvents();

  std::vector<std::weak_ptr<CustomMouseInteractable>> mouseEventListeners;
  std::vector<std::weak_ptr<CustomKeyboardInteractable>> keyboardEventListeners;

  std::set<Window::EventType> keyboardEvents{SDL_KEYDOWN, SDL_KEYUP, SDL_TEXTEDITING, SDL_TEXTINPUT, SDL_KEYMAPCHANGED};
  std::set<Window::EventType> mouseEvents{SDL_MOUSEMOTION, SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP, SDL_MOUSEWHEEL};
};
} // namespace sdl2cpp::ui

#endif // TERRAINGENERATION_EVENTDISPATCHER_H
