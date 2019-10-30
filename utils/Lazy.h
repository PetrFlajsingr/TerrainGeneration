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
    return value();
  }

  const T& value() {
    if (!data.has_value()) {
      data = calcFunction();
    }
    return data.value();
  }

  void invalidate() {
    data = std::nullopt;
  }
private:
  CalcFunction calcFunction;
  std::optional<T> data;
};

#endif // TERRAINGENERATION_LAZY_H
