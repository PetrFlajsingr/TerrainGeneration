//
// Created by petr on 10/11/19.
//

#ifndef TERRAINGENERATION_EVENTDISPATCHER_H
#define TERRAINGENERATION_EVENTDISPATCHER_H

#include "ui/Fwd.h"
#include "ui/interface/KeyboardInteractable.h"
#include "ui/interface/MouseInteractable.h"
#include <SDL2CPP/Window.h>
#include <list>
#include <meta/meta.h>
#include <set>

namespace sdl2cpp::ui {

using namespace std::chrono_literals;

class TimedEvent {
  enum class Type { SingleShot, Repeated, Infinite };
  template <SimpleInvocable F>
  explicit TimedEvent(F f, Type type, std::chrono::milliseconds duration, unsigned int repetitions)
      : event(f), duration(duration), type(type), repetitions(repetitions) {}
public:
  /**
   * Create a TimedEvent instance which fires only once after given time.
   * @param f function to run
   * @param duration delay for event fire
   */
  template <SimpleInvocable F>
  static TimedEvent SingleShot(F f, std::chrono::milliseconds duration) {
    TimedEvent result{std::move(f), Type::SingleShot, duration, 1};
    return result;
  }
  /**
   * Create a TimedEvent instance which fires repeatedly in intervals.
   * @param f function to run
   * @param duration delay for event fire
   * @param repetitions amount of event fires
   */
  template <SimpleInvocable F>
  static TimedEvent Repeated(F f, std::chrono::milliseconds duration, unsigned int repetitions) {
    TimedEvent result{std::move(f), Type::Repeated, duration, repetitions};
    return result;
  }
  /**
   * Create a TimedEvent instance which fires infinitely in intervals.
   * @param f function to run
   * @param duration delay for event fire
   */
  template <SimpleInvocable F>
  static TimedEvent Infinite(F f, std::chrono::milliseconds duration) {
    TimedEvent result{std::move(f), Type::Infinite, duration, 0};
    return result;
  }
  /**
   * Check if event should fire based on its type and time.
   * @param time current timestamp
   */
  [[nodiscard]] bool shouldFire(std::chrono::milliseconds time);
  /**
   * TimedEvent is invalid when it should no longer fire (SingleShot, Repeated) or when it is invalidated.
   */
  [[nodiscard]] bool isValid();
  /**
   * Enable/disable event. Does not remove event, only pauses it.
   */
  void setEnabled(bool enabled);
  [[nodiscard]] bool isEnabled() const;
  /**
   * Mark event as invalid - it will no longer fire and will be removed.
   */
  void invalidate();
  /**
   * Fire the event.
   * @param now current timestamp
   */
  void operator()(std::chrono::milliseconds now);

private:
  std::function<void()> event;
  std::chrono::milliseconds duration;
  std::chrono::milliseconds start = now<std::chrono::milliseconds>();
  Type type;
  unsigned int repetitions;
  unsigned int fireCount = 0;
  bool valid = true;
  bool enabled = true;
};

class EventDispatcher {
  friend class UIManager;

public:
  explicit EventDispatcher(std::shared_ptr<Window> window,
                           FocusManager &focusManager);

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

  void addMouseEventListener(
      const std::weak_ptr<CustomMouseInteractable> &mouseInteractable);
  void addKeyboardEventListener(
      const std::weak_ptr<CustomKeyboardInteractable> &keyboardInteractable);

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
   * @return mouse interactable on position or std::nullopt if none was found on given position
   */
  std::optional<std::shared_ptr<CustomMouseInteractable>>
  findMouseInteractableOnPosition(int x, int y);
  /**
   * Find keyboard interactable currently in focus.
   * @return focused keyobard interactable or std::nullopt if no element has focus
   */
  std::optional<std::shared_ptr<CustomKeyboardInteractable>>
  getFocusedKeyboardInteractable();

  void registerWindowEvents();

  std::vector<std::weak_ptr<CustomMouseInteractable>> mouseEventListeners;
  std::vector<std::weak_ptr<CustomKeyboardInteractable>> keyboardEventListeners;

  std::set<Window::EventType> keyboardEvents{SDL_KEYDOWN, SDL_KEYUP,
                                             SDL_TEXTEDITING, SDL_TEXTINPUT,
                                             SDL_KEYMAPCHANGED};
  std::set<Window::EventType> mouseEvents{SDL_MOUSEMOTION, SDL_MOUSEBUTTONDOWN,
                                          SDL_MOUSEBUTTONUP, SDL_MOUSEWHEEL};
};
} // namespace sdl2cpp::ui

#endif // TERRAINGENERATION_EVENTDISPATCHER_H
