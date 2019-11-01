//
// Created by petr on 10/31/19.
//

#include "exceptions.h"
using namespace std::string_literals;
exc::Throwable::Throwable(std::string_view msg, std::experimental::source_location srcLoc)
    : what_(std::string(msg) + "\nThrown from file: " + srcLoc.file_name() +
            "\nfunction: " + srcLoc.function_name() +
            "\nline: " + std::to_string(srcLoc.line()) +
            "\ncolumn: " + std::to_string(srcLoc.column())) {}

const char *exc::Throwable::what() const noexcept {
  return what_.data();
}

exc::ProgrammingError::ProgrammingError(std::string_view msg)
    : Error("FIX THIS: "s + std::string(msg)) {}
