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
  template <SimpleInvocable F>
  static TimedEvent SingleShot(F f, std::chrono::milliseconds duration) {
    TimedEvent result{std::move(f), Type::SingleShot, duration, 1};
    return result;
  }
  template <SimpleInvocable F>
  static TimedEvent Repeated(F f, std::chrono::milliseconds duration, unsigned int repetitions) {
    TimedEvent result{std::move(f), Type::Repeated, duration, repetitions};
    return result;
  }
  template <SimpleInvocable F>
  static TimedEvent Infinite(F f, std::chrono::milliseconds duration) {
    TimedEvent result{std::move(f), Type::Infinite, duration, 0};
    return result;
  }

  [[nodiscard]] bool shouldFire(std::chrono::milliseconds time) {
    bool result = time - start >= duration;
    valid = !result || type == Type::Infinite || (type == Type::Repeated && (repetitions > fireCount));
    return result;
  }

  [[nodiscard]] bool isValid() { return valid; }

  void operator()(std::chrono::milliseconds now) {
    start = now;
    ++fireCount;
    event();
  }

private:
  std::function<void()> event;
  std::chrono::milliseconds duration;
  std::chrono::milliseconds start = now<std::chrono::milliseconds>();
  Type type;
  unsigned int repetitions;
  unsigned int fireCount = 0;
  bool valid = true;
};

class EventDispatcher {
  friend class UIManager;

public:
  explicit EventDispatcher(std::shared_ptr<Window> window,
                           FocusManager &focusManager);


  void addEvent(TimedEvent &&event);

  void checkTimedEvents(std::chrono::milliseconds currentTime);

private:
  std::list<TimedEvent> events;
  bool doubleClicked = false;

  FocusManager &focusManager;
  void addMouseEventListener(
      const std::weak_ptr<CustomMouseInteractable> &mouseInteractable);
  void addKeyboardEventListener(
      const std::weak_ptr<CustomKeyboardInteractable> &keyboardInteractable);

  bool mouseEventHandler(const SDL_Event &event);

  bool keyboardEventHandler(const SDL_Event &event);

  CustomMouseInteractable *mouseDownIn = nullptr;
  CustomMouseInteractable *mouseIn = nullptr;

  std::optional<std::shared_ptr<CustomMouseInteractable>>
  findMouseInteractibleOnPosition(int x, int y);

  std::optional<std::shared_ptr<CustomKeyboardInteractable>>
  getFocusedKeyboardInteractible();

  std::shared_ptr<Window> window;
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
