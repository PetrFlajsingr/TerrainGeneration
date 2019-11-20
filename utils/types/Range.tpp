
template <typename T>
template <typename U, typename V>
Range<T>::Range(T start, U end, V step)
    : _start(start), _end(end), _step(step) {}

template <typename T> typename Range<T>::iterator Range<T>::begin() {
  return iterator{_start, _step, _end};
}

template <typename T> typename Range<T>::iterator Range<T>::end() {
  return iterator{_end, _step, _end};
}

template <typename T>
Range<T>::iterator::iterator(T value, T step, T max)
    : value(value), step(step), max(max) {}

template <typename T>
Range<T>::iterator::iterator(const Range::iterator &other) {
  value = other.value;
}

template <typename T>
typename Range<T>::iterator &
Range<T>::iterator::operator=(const Range::iterator &other) {
  value = other.value;
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
  if (value > max) {
    value = max;
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
  return Range{start, end, step};
}

template <typename T, typename U, typename V>
Range<T> MakeRange::range(T start, U end, V step) {
  return Range{start, end + 1, step};
}

template <typename T, typename U, typename V>
Range<T> MakeRange::downTo(T start, U end, V step) {
  return Range{end, start, -step};
}