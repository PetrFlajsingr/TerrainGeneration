//
// Created by petr on 10/26/19.
//

#ifndef LAZY_H
#define LAZY_H

#include <functional>
#include <optional>

template <typename T> class Lazy {
public:
  using CalcFunction = std::function<T()>;
  explicit Lazy(CalcFunction &&calcFunction);
  operator const T &();
  const T &value();
  void invalidate();

private:
  CalcFunction calcFunction;
  std::optional<T> data;
};

template <typename T> std::ostream &operator<<(std::ostream &stream, Lazy<T> &other);

#include "Lazy.tpp"

#endif // LAZY_H
