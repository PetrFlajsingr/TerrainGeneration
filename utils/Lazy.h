//
// Created by petr on 10/26/19.
//

#ifndef TERRAINGENERATION_LAZY_H
#define TERRAINGENERATION_LAZY_H

#include <functional>
#include <optional>

template <typename T>
class Lazy {
public:
  using CalcFunction = std::function<T()>;
  explicit Lazy(CalcFunction &&calcFunction) : calcFunction(calcFunction) {}

  operator const T&() {
    if (!value.has_value()) {
      value = calcFunction();
    }
    return value.value();
  }

  void invalidate() {
    value = std::nullopt;
  }
private:
  CalcFunction calcFunction;
  std::optional<T> value;
};

#endif // TERRAINGENERATION_LAZY_H
