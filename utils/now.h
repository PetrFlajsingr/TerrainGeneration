//
// Created by petr on 10/28/19.
//

#ifndef TERRAINGENERATION_NOW_H
#define TERRAINGENERATION_NOW_H
#include <chrono>
#include <meta.h>

template<typename T, typename = std::enable_if_t<is_duration_v<T>>>
inline auto now() {
  return std::chrono::duration_cast<T>(std::chrono::system_clock::now().time_since_epoch());
}

#endif // TERRAINGENERATION_NOW_H
