//
// Created by petr on 12/14/19.
//

#include "TimedEvent.h"

bool sdl2cpp::ui::TimedEvent::shouldFire(std::chrono::milliseconds time) {
  if (!isValid()) {
    return false;
  }
  bool result = time - start >= duration;
  valid = !result || type == Type::Infinite || (type == Type::Repeated && (repetitions > fireCount));
  return result && enabled;
}

bool sdl2cpp::ui::TimedEvent::isValid() { return valid; }

void sdl2cpp::ui::TimedEvent::setEnabled(bool enabled) { TimedEvent::enabled = enabled; }

bool sdl2cpp::ui::TimedEvent::isEnabled() const { return enabled; }

void sdl2cpp::ui::TimedEvent::invalidate() { valid = false; }

void sdl2cpp::ui::TimedEvent::operator()(std::chrono::milliseconds now) {
  start = now;
  ++fireCount;
  event();
}
