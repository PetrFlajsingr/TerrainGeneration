//
// Created by petr on 11/21/19.
//

#ifndef UTILITIES_BINDING_H
#define UTILITIES_BINDING_H

#include "OneWayBinding.h"
#include "TwoWayBinding.h"

template <typename T, typename U, typename F1 = std::function<U(const T &)>, typename F2 = std::function<T(const U &)>>
auto bindBidirectional(observable::value<T> &a, observable::value<U> &b,
                       F1 transformA = F1{[](const T &val) -> U { return U(val); }},
                       F2 transformB = F2{[](const U &val) -> T { return T(val); }}) {
  return TwoWayBinding{a, b, transformA, transformB};
}

template <typename T, typename U, typename F1 = std::function<U(const T &)>>
auto bindTo(observable::value<T> &a, observable::value<U> &b, F1 transformB = F1{[](const U &val) -> U { return T(val); }}) {
  return OneWayBinding{a, b, transformB};
}

template <typename T, typename U, typename F1 = std::function<U(const T &)>,
          typename = typename std::enable_if_t<!is_specialization_v<T, observable::value>>>
auto bindTo(T &a, observable::value<U> &b, F1 transformB = F1{[](const U &val) -> U { return T(val); }}) {
  return OneWayBindingVal<T, U>{a, b, transformB};
}

#endif // UTILITIES_BINDING_H
