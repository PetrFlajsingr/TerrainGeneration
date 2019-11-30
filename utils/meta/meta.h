//
// Created by petr on 10/28/19.
//

#ifndef TERRAINGENERATION_META_H
#define TERRAINGENERATION_META_H

#include <chrono>
#include <type_traits>
#include <glm/glm.hpp>
#include "iterable.h"
#include "specializations.h"
#include "chrono.h"
#include "string.h"

template<typename T1, typename... T>
struct first_of {
    using type = T1;
};
template<typename... T>
using first_of_t = typename first_of<T...>::type;

template<typename T, typename... Types>
constexpr bool is_one_of_v = (std::is_same_v<T, Types> || ...);

template<template<class> typename T, typename... Types>
constexpr bool any_of_v = (T<Types>::value || ...);

template<typename T>
struct tag {
    using type = T;
};
template<typename... Ts>
struct last_of {
    using type = typename decltype((tag<Ts>{}, ...))::type;
};
template<typename... T>
using last_of_t = typename last_of<T...>::type;

template<typename ...T>
using highest_precision_t = std::conditional_t<is_one_of_v<long double, T...>, long double, std::conditional_t<is_one_of_v<double, T...>, double, std::conditional_t<is_one_of_v<float, T...>, float, void>>>;

template<typename ...T>
using largest_integer_t = std::conditional_t<is_one_of_v<long long, T...>, long long,
        std::conditional_t<is_one_of_v<unsigned long long, T...>, unsigned long long,
                std::conditional_t<is_one_of_v<long, T...>, long,
                        std::conditional_t<is_one_of_v<unsigned long, T...>, unsigned long,
                                std::conditional_t<is_one_of_v<int, T...>, int,
                                        std::conditional_t<is_one_of_v<unsigned int, T...>, unsigned int,
                                                std::conditional_t<is_one_of_v<unsigned short, T...>, unsigned short,
                                                        std::conditional_t<is_one_of_v<short, T...>, short,
                                                                std::conditional_t<is_one_of_v<char32_t, T...>, char32_t,
                                                                        std::conditional_t<is_one_of_v<char16_t, T...>, char16_t,
                                                                                std::conditional_t<is_one_of_v<wchar_t, T...>, wchar_t,
                                                                                        std::conditional_t<is_one_of_v<signed char, T...>, signed char,
                                                                                                std::conditional_t<is_one_of_v<unsigned char, T...>, unsigned char,
                                                                                                        std::conditional_t<is_one_of_v<char, T...>, char, void>>>>>>>>>>>>>>;

template<typename ...T>
using largest_signed_integer_t = std::conditional_t<is_one_of_v<long long, T...>, long long,
        std::conditional_t<is_one_of_v<long, T...>, long,
                std::conditional_t<is_one_of_v<int, T...>, int,
                        std::conditional_t<is_one_of_v<short, T...>, short,
                                std::conditional_t<is_one_of_v<char32_t, T...>, char32_t,
                                        std::conditional_t<is_one_of_v<char16_t, T...>, char16_t,
                                                std::conditional_t<is_one_of_v<wchar_t, T...>, wchar_t,
                                                        std::conditional_t<is_one_of_v<signed char, T...>, signed char,
                                                                std::conditional_t<is_one_of_v<char, T...>, char, void>>>>>>>>>;
#endif // TERRAINGENERATION_META_H
