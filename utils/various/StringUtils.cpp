//
// Created by petr on 11/20/19.
//

#include "StringUtils.h"

std::string StringUtils::getRandomString(size_t length) {
  auto randChar = []() -> char {
    const char charset[] = "0123456789"
                           "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                           "abcdefghijklmnopqrstuvwxyz";
    const size_t max_index = (sizeof(charset) - 1);
    return charset[rand() % max_index];
  };
  std::string str(length, 0);
  std::generate_n(str.begin(), length, randChar);
  return str;
}

std::string StringUtils::toLower(const std::string &str) {
  std::string result;
  std::transform(str.begin(), str.end(), std::back_inserter(result), ::tolower);
  return result;
}

std::string StringUtils::toUpper(const std::string &str) {
  std::string result;
  std::transform(str.begin(), str.end(), std::back_inserter(result), ::toupper);
  return result;
}