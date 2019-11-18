//
// Created by Petr on 18.11.2019.
//

#ifndef UTILITIES_JSONCONFIG_H
#define UTILITIES_JSONCONFIG_H

#include "Config.h"
#include "types/Range.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <print.h>

template <>
struct ConfigContainerTraits<nlohmann::json> {
    template <typename T, typename ...Keys>
    static std::optional<T> find(nlohmann::json &container, const Keys &...keys) {
      using namespace MakeRange;
      std::vector<std::string> tmpKeys{keys...};
      nlohmann::json::iterator iter;
      for (uint i : until(tmpKeys.size())) {
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
      return static_cast<T>(*iter);
    }
    template <typename T, typename Key>
    static bool contains(nlohmann::json &container, const Key &key) {
        return container.contains(key);
    }
    template <typename T, typename Key>
    static void set(nlohmann::json &container, const Key &key, T &&value) {
        container[key] = value;
    }
};


template <>
struct ConfigLoader<nlohmann::json> {
    nlohmann::json load(std::string_view path) {
        std::ifstream stream{std::string(path)};
        nlohmann::json result;
        stream >> result;
        return result;
    }
};

template <>
struct ConfigSaver<nlohmann::json> {
    void save(nlohmann::json &config, std::string_view path) {
        std::ofstream stream{std::string(path)};
        stream << config;
    }
};

template <bool ReadOnly>
using JsonConfig = Config<nlohmann::json, ReadOnly>;

#endif //UTILITIES_JSONCONFIG_H
