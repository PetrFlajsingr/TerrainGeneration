//
// Created by petr on 10/28/19.
//

#ifndef TERRAINGENERATION_NOW_H
#define TERRAINGENERATION_NOW_H
#include "../meta/chrono.h"
#include <chrono>

template <Duration T> inline auto now() {
  using namespace std::chrono;
  return duration_cast<T>(system_clock::now().time_since_epoch());
}

#endif // TERRAINGENERATION_NOW_H
