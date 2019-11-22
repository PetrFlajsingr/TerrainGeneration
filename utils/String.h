//
// Created by petr on 11/20/19.
//

#ifndef TERRAINGENERATION_STRING_H
#define TERRAINGENERATION_STRING_H

#include "various/StringUtils.h"
using String = StringDecorator<std::string>;
using WString = StringDecorator<std::wstring>;

inline String operator""_s(const char *val, size_t) { return String{val}; }

inline WString operator""_sw(const wchar_t *val, size_t) {
  return WString{val};
}

#endif // TERRAINGENERATION_STRING_H
