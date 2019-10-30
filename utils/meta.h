//
// Created by petr on 10/28/19.
//

#ifndef TERRAINGENERATION_META_H
#define TERRAINGENERATION_META_H

#include <type_traits>
#include <chrono>

template <class T, template <class...> class Template>
struct is_specialization : std::false_type {};

template <template <class...> class Template, class... Args>
struct is_specialization<Template<Args...>, Template> : std::true_type {};

template <typename T, template <class...> class U>
static inline constexpr bool is_specialization_v = is_specialization<T, U>::value;

template <typename T>
static inline constexpr bool is_duration_v = is_specialization<T, std::chrono::duration>::value;


template <typename T, typename... Types>
static inline constexpr bool is_one_of_v = (std::is_same_v<T, Types> || ... || false);

#endif // TERRAINGENERATION_META_H
