//
// Created by petr on 10/28/19.
//

#ifndef TERRAINGENERATION_META_H
#define TERRAINGENERATION_META_H

#include <chrono>
#include <type_traits>

template <class T, template <class...> class Template>
struct is_specialization : std::false_type {};

template <template <class...> class Template, class... Args>
struct is_specialization<Template<Args...>, Template> : std::true_type {};

template <typename T, template <class...> class U>
static inline constexpr bool is_specialization_v =
    is_specialization<T, U>::value;

template <typename T>
static inline constexpr bool is_duration_v =
    is_specialization<T, std::chrono::duration>::value;

template <typename T, typename... Types>
static inline constexpr bool is_one_of_v = (std::is_same_v<T, Types> || ... ||
                                            false);

template <typename T, typename = void> struct is_iterable : std::false_type {};
template <typename T>
struct is_iterable<T, std::void_t<decltype(std::begin(std::declval<T>())),
                                  decltype(std::end(std::declval<T>()))>>
    : std::true_type {};

template <typename T> constexpr bool is_iterable_v = is_iterable<T>::value;

template <class T, template <class, glm::precision> class Template>
struct is_vec_specialization : std::false_type {};
template <template <class, glm::precision> class Template, typename ValueType,
          glm::precision P>
struct is_vec_specialization<Template<ValueType, P>, Template>
    : std::true_type {};

template <typename T>
constexpr bool is_vec_specialisation_v =
    is_vec_specialization<std::decay_t<T>, glm::tvec4>::value ||
    is_vec_specialization<std::decay_t<T>, glm::tvec3>::value ||
    is_vec_specialization<std::decay_t<T>, glm::tvec2>::value;

#endif // TERRAINGENERATION_META_H
