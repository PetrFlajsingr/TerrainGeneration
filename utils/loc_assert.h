//
// Created by petr on 11/18/19.
//

#ifndef TERRAINGENERATION_LOC_ASSERT_H
#define TERRAINGENERATION_LOC_ASSERT_H

#include "print.h"
#include <experimental/source_location>

inline void loc_assert(bool cond, std::string msg = "", std::experimental::source_location sourceLocation = std::experimental::source_location::current()) {
  if (!cond) {
    printErr("Assert error on ", sourceLocation.file_name(), ", ", sourceLocation.function_name(), ", ", sourceLocation.line());
    printErr(msg);
    std::terminate();
  }
}

#endif // TERRAINGENERATION_LOC_ASSERT_H
