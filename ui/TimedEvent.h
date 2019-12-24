//
// Created by petr on 12/14/19.
//

#ifndef TERRAINGENERATION_TIMEDEVENT_H
#define TERRAINGENERATION_TIMEDEVENT_H

#include "meta/meta.h"
#include "time/now.h"
#include <chrono>
#include <utility>

namespace sdl2cpp::ui {

using namespace std::chrono_literals;

class TimedEvent {
  enum class Type { SingleShot, Repeated, Infinite };
  template <SimpleInvocable F> explicit TimedEvent(F f, Type type, std::chrono::milliseconds duration, unsigned int repetitions)
      : event(f), duration(duration), type(type), repetitions(repetitions) {}

public:
  /**
   * Create a TimedEvent instance which fires only once after given time.
   * @param f function to run
   * @param duration delay for event fire
   */
  template <SimpleInvocable F> static TimedEvent SingleShot(F f, std::chrono::milliseconds duration) {
    TimedEvent result{std::move(f), Type::SingleShot, duration, 1};
    return result;
  }
  /**
   * Create a TimedEvent instance which fires repeatedly in intervals.
   * @param f function to run
   * @param duration delay for event fire
   * @param repetitions amount of event fires
   */
  template <SimpleInvocable F> static TimedEvent Repeated(F f, std::chrono::milliseconds duration, unsigned int repetitions) {
    TimedEvent result{std::move(f), Type::Repeated, duration, repetitions};
    return result;
  }
  /**
   * Create a TimedEvent instance which fires infinitely in intervals.
   * @param f function to run
   * @param duration delay for event fire
   */
  template <SimpleInvocable F> static TimedEvent Infinite(F f, std::chrono::milliseconds duration) {
    TimedEvent result{std::move(f), Type::Infinite, duration, 0};
    return result;
  }
  /**
   * Check if event should fire based on its type and time.
   * @param time current timestamp
   */
  [[nodiscard]] bool shouldFire(std::chrono::milliseconds time);
  /**
   * TimedEvent is invalid when it should no longer fire (SingleShot, Repeated)
   * or when it is invalidated.
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
} // namespace sdl2cpp::ui

#endif // TERRAINGENERATION_TIMEDEVENT_H
