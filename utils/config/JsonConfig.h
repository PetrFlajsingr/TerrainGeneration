//
// Created by Petr on 18.11.2019.
//

#ifndef UTILITIES_JSONCONFIG_H
#define UTILITIES_JSONCONFIG_H

#include "Config.h"
#include "types/Range.h"
#include <fstream>
#include <nlohmann/json.hpp>

namespace {
using namespace MakeRange;

template <typename... Keys>
std::optional<nlohmann::json::iterator> findJsonNode(nlohmann::json &container,
                                                     const Keys &... keys) {
  std::vector<std::string> tmpKeys{keys...};
  nlohmann::json::iterator iter;
  for (unsigned int i : range(0, tmpKeys.size())) {
    if (i == 0) {
      if (iter = container.find(tmpKeys[i]); iter == container.end()) {
        return std::nullopt;
      }
    } else {
      const auto tmpEnd = iter->end();
      if (iter = iter->find(tmpKeys[i]); iter == tmpEnd) {
        return std::nullopt;
      }
    }
  }
  return iter;
}

template <typename T, typename... Keys>
void setJsonNode(nlohmann::json &container, const T &value,
                 const Keys &... keys) {
  std::vector<std::string> tmpKeys{keys...};
  auto tmp = container;
  for (unsigned int i : range(0, tmpKeys.size() - 1)) {
    if (!tmp.contains(tmpKeys[i])) {
      tmp[tmpKeys[i]] = nlohmann::json::object();
    }
    tmp = tmp[tmpKeys[i]];
  }
  tmp[tmpKeys.back()] = value;
}
} // namespace

template <> struct ConfigContainerTraits<nlohmann::json> {
  template <typename T, typename... Keys>
  static std::optional<T> find(nlohmann::json &container,
                               const Keys &... keys) {
    if (auto iter = findJsonNode(container, keys...); iter.has_value()) {
      return **iter;
    }
    return std::nullopt;
  }

  template <typename T, typename... Keys>
  static bool contains(nlohmann::json &container, const T &value,
                       const Keys &... keys) {
    return findJsonNode(container, value, keys...).has_value();
  }

  template <typename T, typename... Keys>
  static void set(nlohmann::json &container, T &&value, const Keys &... keys) {
    setJsonNode(container, value, keys...);
  }
};

template <> struct ConfigLoader<nlohmann::json> {
  nlohmann::json load(std::string_view path) {
    std::ifstream stream{std::string(path)};
    nlohmann::json result;
    stream >> result;
    return result;
  }
};

template <> struct ConfigSaver<nlohmann::json> {
  void save(nlohmann::json &config, std::string_view path) {
    std::ofstream stream{std::string(path)};
    stream << config;
  }
};

template <bool ReadOnly> using JsonConfig = Config<nlohmann::json, ReadOnly>;

#endif // UTILITIES_JSONCONFIG_H
