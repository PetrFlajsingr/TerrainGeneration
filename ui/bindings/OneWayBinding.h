//
// Created by petr on 11/21/19.
//

#ifndef UTILITIES_ONEWAYBINDING_H
#define UTILITIES_ONEWAYBINDING_H

#include <functional>
#include <include/observable/value.hpp>

template <typename T, typename U> class OneWayBinding {
  using TransformBtoAfnc = std::function<T(const U &)>;
  using UnsubscriberB =
      decltype(std::declval<observable::value<U>>().subscribe(std::declval<std::function<void(const U &val)>>()));

public:
  template <typename F1 = TransformBtoAfnc> OneWayBinding(observable::value<T> &a, observable::value<U> &b,
                                                          F1 transformB = TransformBtoAfnc{[](const U &val) { return T(val); }})
      : a(a), b(b), tA(transformB) {
    auto fncAtoB = [this](const T &val) { this->a = tA(val); };
    unsubscriberB = b.subscribe(fncAtoB);
  }

  ~OneWayBinding() { unsubscriberB.unsubscribe(); }

private:
  observable::value<T> &a;
  observable::value<U> &b;

  UnsubscriberB unsubscriberB;

  TransformBtoAfnc tA;
};

template <typename T, typename U> class OneWayBindingVal {
  using TransformAtoBfnc = std::function<U(const T &)>;
  using UnsubscriberB =
      decltype(std::declval<observable::value<U>>().subscribe(std::declval<std::function<void(const U &val)>>()));

public:
  template <typename F1 = TransformAtoBfnc>
  OneWayBindingVal(T &a, observable::value<T> &b, F1 transformA = TransformAtoBfnc{[](const T &val) -> U { return U(val); }})
      : a(a), b(b), tA(transformA) {
    unsubscriberB = b.subscribe([this](const U &val) { this->a = val; });
  }

  ~OneWayBindingVal() { unsubscriberB.unsubscribe(); }

private:
  T &a;
  observable::value<U> &b;

  UnsubscriberB unsubscriberB;

  TransformAtoBfnc tA;
};

#endif // UTILITIES_ONEWAYBINDING_H
