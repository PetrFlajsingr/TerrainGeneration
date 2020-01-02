//
// Created by Petr on 18.11.2019.
//

#ifndef UTILITIES_CHRONO_H
#define UTILITIES_CHRONO_H

#include "specializations.h"
#include <chrono>

template <typename T> static inline constexpr bool is_duration_v = is_specialization<T, std::chrono::duration>::value;

template <typename T> concept Duration = is_duration_v<T>;

template <Duration Dur> constexpr auto durationToString() {
  using namespace std::chrono;
  if constexpr (std::is_same_v<Dur, nanoseconds>) {
    return "ns";
  } else if constexpr (std::is_same_v<Dur, microseconds>) {
    return "us";
  } else if constexpr (std::is_same_v<Dur, milliseconds>) {
    return "ms";
  } else if constexpr (std::is_same_v<Dur, seconds>) {
    return "s";
  } else if constexpr (std::is_same_v<Dur, minutes>) {
    return "m";
  } else if constexpr (std::is_same_v<Dur, hours>) {
    return "h";
  }
  return "";
}
#endif // UTILITIES_CHRONO_H
