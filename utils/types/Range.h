//
// Created by Petr on 18.11.2019.
//

#ifndef UTILITIES_RANGE_H
#define UTILITIES_RANGE_H

template <typename T> class Range {
private:
  struct iterator {
    iterator() = default;

    iterator(T value, T step, T max);

    iterator(const iterator &other);

    iterator &operator=(const iterator &other);

    bool operator==(const iterator &rhs) const;

    bool operator!=(const iterator &rhs) const;

    T operator*() const;

    T *operator->();

    iterator &operator++();

    iterator operator++(int);

    T value;
    T step;
    T max;
  };

public:
  template <typename U = T, typename V = T>
  Range(T start, U end, V step = V{1});

  iterator begin();

  iterator end();

private:
  const T _start;
  const T _end;
  const T _step;
};

namespace MakeRange {
template <typename T, typename U = T, typename V = T>
Range<T> until(T start, U end, V step = V{1});

template <typename T, typename U = T, typename V = T>
Range<T> range(T start, U end, V step = V{1});

template <typename T, typename U = T, typename V = T>
Range<T> downTo(T start, U end, V step = V{1});
} // namespace MakeRange

#include "Range.tpp"
#endif // UTILITIES_RANGE_H
