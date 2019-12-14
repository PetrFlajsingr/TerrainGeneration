template <typename T>
template <typename U, typename V>
Range<T>::Range(T start, U end, V step)
    : _start(start), _end(end), _step(step) {}

template <typename T> typename Range<T>::iterator Range<T>::begin() const {
  return iterator{_start, _step, _end, _start <= _end};
}

template <typename T> typename Range<T>::iterator Range<T>::end() const {
  return iterator{_end, _step, _end, _start <= _end};
}

template <typename T>
Range<T>::iterator::iterator(T value, T step, T endValue, bool up)
    : value(value), step(step), endValue(endValue), up(up) {}

template <typename T>
Range<T>::iterator::iterator(const Range::iterator &other) {
  value = other.value;
  endValue = other.endValue;
  step = other.step;
  up = other.up;
}

template <typename T>
typename Range<T>::iterator &
Range<T>::iterator::operator=(const Range::iterator &other) {
  value = other.value;
  endValue = other.endValue;
  step = other.step;
  up = other.up;
}

template <typename T>
bool Range<T>::iterator::operator==(const Range::iterator &rhs) const {
  return value == rhs.value;
}

template <typename T>
bool Range<T>::iterator::operator!=(const Range::iterator &rhs) const {
  return !(rhs == *this);
}

template <typename T> T Range<T>::iterator::operator*() const { return value; }

template <typename T> T *Range<T>::iterator::operator->() { return &value; }

template <typename T>
typename Range<T>::iterator &Range<T>::iterator::operator++() {
  value += step;
  if (up && value > endValue) {
    value = endValue;
  } else if (!up && value < endValue) {
    value = endValue;
  }
  return *this;
}

template <typename T>
typename Range<T>::iterator Range<T>::iterator::operator++(int) {
  iterator tmp{*this};
  operator++();
  return tmp;
}

template <typename T, typename U, typename V>
Range<T> MakeRange::until(T start, U end, V step) {
  assert(step >= V{0});
  assert(start <= end);
  return Range{start, end, step};
}

template <typename T, typename U, typename V>
Range<T> MakeRange::to(T start, U end, V step) {
  assert(step >= V{0});
  assert(start <= end);
  return Range{start, end + 1, step};
}

template <typename T, typename U, typename V>
Range<T> MakeRange::downTo(T start, U end, V step) {
  assert(step >= V{0});
  assert(start >= end);
  return Range{start, end - 1, -step};
}

template <typename T, typename U, typename V>
Range<T> MakeRange::downUntil(T start, U end, V step) {
  assert(step >= V{0});
  assert(start >= end);
  return Range{start, end, -step};
}

template <typename T, typename U, typename V, typename ValueType>
Range<ValueType> MakeRange::range(T start, U end, V step) {
  if (step == V{0}) {
    step = std::numeric_limits<V>::max();
  }
  if (static_cast<ValueType>(step) < static_cast<ValueType>(V{0})) {
    assert(static_cast<ValueType>(start) >= static_cast<ValueType>(end));
    return Range<ValueType>{static_cast<ValueType>(start),
                            static_cast<ValueType>(end),
                            static_cast<ValueType>(step)};
  } else {
    assert(static_cast<ValueType>(start) <= static_cast<ValueType>(end));
    return Range<ValueType>{static_cast<ValueType>(start),
                            static_cast<ValueType>(end),
                            static_cast<ValueType>(step)};
  }
}

template <typename T> Range<T> MakeRange::range(T start) {
  const auto end = T{0};
  if (start <= end) {
    return Range{end, start, -T{1}};
  } else {
    return Range{end, start, T{1}};
  }
}

template <typename T, unsigned int Dimensions>
MultiDimRange<T, Dimensions>
MakeRange::range(typename MultiDimRange<T, Dimensions>::value_type start,
                 typename MultiDimRange<T, Dimensions>::value_type end,
                 typename MultiDimRange<T, Dimensions>::value_type step) {
  std::for_each(step.begin(), step.end(), [](auto &val) {
    if (val == 0) {
      val = std::numeric_limits<T>::max();
    }
  });
  for (unsigned int i = 0; i < Dimensions; ++i) {
    assert((start[i] < end[i]) == (step[i] > 0));
  }
  return MultiDimRange<T, Dimensions>{start, end, step};
}

template <typename T, unsigned int Dimensions>
MultiDimRange<T, Dimensions>
MakeRange::range(typename MultiDimRange<T, Dimensions>::value_type end) {
  std::array<T, Dimensions> step;
  std::fill(step.begin(), step.end(), T{1});
  return MultiDimRange<T, Dimensions>{std::array<T, Dimensions>{T{}}, end,
                                      step};
}

template <typename T, unsigned int Dimensions>
MultiDimRange<T, Dimensions>::iterator::iterator(
    MultiDimRange::value_type value, MultiDimRange::value_type step,
    MultiDimRange::value_type startValue, MultiDimRange::value_type endValue)
    : value(value), step(step), startValue(startValue), endValue(endValue) {
  for (unsigned int i = 0; i < Dimensions; ++i) {
    if (startValue[i] < endValue[i]) {
      comp[i] = std::less<T>{};
    } else {
      comp[i] = std::greater<T>{};
    }
  }
}

template <typename T, unsigned int Dimensions>
MultiDimRange<T, Dimensions>::iterator::iterator(
    const MultiDimRange::iterator &other) {
  value = other.value;
  startValue = other.startValue;
  endValue = other.endValue;
  step = other.step;
  comp = other.comp;
}

template <typename T, unsigned int Dimensions>
typename MultiDimRange<T, Dimensions>::iterator &
MultiDimRange<T, Dimensions>::iterator::operator=(
    const MultiDimRange::iterator &other) {
  value = other.value;
  startValue = other.startValue;
  endValue = other.endValue;
  step = other.step;
  comp = other.comp;
}

template <typename T, unsigned int Dimensions>
bool MultiDimRange<T, Dimensions>::iterator::operator==(
    const MultiDimRange::iterator &rhs) const {
  return value == rhs.value;
}

template <typename T, unsigned int Dimensions>
bool MultiDimRange<T, Dimensions>::iterator::operator!=(
    const MultiDimRange::iterator &rhs) const {
  return !(rhs == *this);
}

template <typename T, unsigned int Dimensions>
typename MultiDimRange<T, Dimensions>::value_type
    MultiDimRange<T, Dimensions>::iterator::operator*() const {
  return value;
}

template <typename T, unsigned int Dimensions>
typename MultiDimRange<T, Dimensions>::value_type *
    MultiDimRange<T, Dimensions>::iterator::operator->() {
  return &value;
}

template <typename T, unsigned int Dimensions>
typename MultiDimRange<T, Dimensions>::iterator &
MultiDimRange<T, Dimensions>::iterator::operator++() {
  bool isLast = true;
  for (unsigned int i = 0; i < Dimensions; ++i) {
    // if (value[i] < endValue[i] - 1) {
    if (comp[i](value[i], endValue[i] - 1)) {
      value[i] += step[i];
      if (i > 0) {
        for (unsigned int j = 0; j < i; ++j)
          value[j] = startValue[j];
      }
      // if (value[i] > endValue[i]) {
      if (!comp[i](value[i], endValue[i])) {
        value[i] = endValue[i];
        continue;
      }
      isLast = false;
      break;
    }
  }
  if (isLast) {
    for (unsigned int i = 0; i < Dimensions; ++i) {
      value[i] = endValue[i];
    }
  }
  return *this;
}

template <typename T, unsigned int Dimensions>
typename MultiDimRange<T, Dimensions>::iterator
MultiDimRange<T, Dimensions>::iterator::operator++(int) {
  iterator tmp{*this};
  operator++();
  return tmp;
}

template <typename T, unsigned int Dimensions>
template <typename U>
MultiDimRange<T, Dimensions>::MultiDimRange(MultiDimRange::value_type start,
                                            MultiDimRange::value_type end,
                                            MultiDimRange::value_type step)
    : _start(start), _end(end), _step(step) {}

template <typename T, unsigned int Dimensions>
typename MultiDimRange<T, Dimensions>::iterator
MultiDimRange<T, Dimensions>::begin() const {
  return iterator{_start, _step, _start, _end};
}

template <typename T, unsigned int Dimensions>
typename MultiDimRange<T, Dimensions>::iterator
MultiDimRange<T, Dimensions>::end() const {
  return iterator{_end, _step, _start, _end};
}
