//
// Created by petr on 11/16/19.
//

#ifndef TERRAINGENERATION_PRINT_H
#define TERRAINGENERATION_PRINT_H

#include <iostream>
#include "logger.h"

namespace detail {
template <typename OutStream, typename ...Args>
void printImpl(OutStream &stream, Args... args) {
  using namespace LoggerStreamModifiers;
  static auto logger = Logger{stream};
  ((logger << verbose()) << ... << args);
}
}
template <typename ...Args>
void print(Args... args) {
  detail::printImpl(std::cout, std::forward<Args>(args)...);
}

template <typename ...Args>
void printErr(Args... args) {
  detail::printImpl(std::cerr, std::forward<Args>(args)...);
}

#endif // TERRAINGENERATION_PRINT_H
