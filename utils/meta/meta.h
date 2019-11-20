//
// Created by petr on 10/28/19.
//

#ifndef TERRAINGENERATION_META_H
#define TERRAINGENERATION_META_H

#include "chrono.h"
#include "iterable.h"
#include "specializations.h"
#include "string.h"
#include <chrono>
#include <glm/glm.hpp>
#include <type_traits>

template <typename T, typename... Types>
constexpr bool is_one_of_v = (std::is_same_v<T, Types> || ...);

template <typename T1, typename... T> struct first_of { using type = T1; };
template <typename... T> using first_of_t = typename first_of<T...>::type;

template <typename T> struct tag { using type = T; };
template <typename... Ts> struct last_of {
  using type = typename decltype((tag<Ts>{}, ...))::type;
};
template <typename... T> using last_of_t = typename last_of<T...>::type;

#endif // TERRAINGENERATION_META_H
