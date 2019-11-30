
#include "Range.h"

template<typename T>
template<typename U, typename V>
Range<T>::Range(T start, U end, V step)
        : _start(start), _end(end), _step(step) {}

template<typename T>
typename Range<T>::iterator Range<T>::begin() {
    return iterator{_start, _step, _end, _start <= _end};
}

template<typename T>
typename Range<T>::iterator Range<T>::end() {
    return iterator{_end, _step, _end, _start <= _end};
}

template<typename T>
Range<T>::iterator::iterator(T value, T step, T endValue, bool up)
        : value(value), step(step), endValue(endValue), up(up) {}

template<typename T>
Range<T>::iterator::iterator(const Range::iterator &other) {
    value = other.value;
}

template<typename T>
typename Range<T>::iterator &
Range<T>::iterator::operator=(const Range::iterator &other) {
    value = other.value;
}

template<typename T>
bool Range<T>::iterator::operator==(const Range::iterator &rhs) const {
    return value == rhs.value;
}

template<typename T>
bool Range<T>::iterator::operator!=(const Range::iterator &rhs) const {
    return !(rhs == *this);
}

template<typename T>
T Range<T>::iterator::operator*() const { return value; }

template<typename T>
T *Range<T>::iterator::operator->() { return &value; }

template<typename T>
typename Range<T>::iterator &Range<T>::iterator::operator++() {
    value += step;
    if (up && value > endValue) {
        value = endValue;
    } else if (!up && value < endValue) {
        value = endValue;
    }
    return *this;
}

template<typename T>
typename Range<T>::iterator Range<T>::iterator::operator++(int) {
    iterator tmp{*this};
    operator++();
    return tmp;
}

template<typename T, typename U, typename V>
Range<T> MakeRange::until(T start, U end, V step) {
    assert(step >= V{0});
    assert(start <= end);
    return Range{start, end, step};
}

template<typename T, typename U, typename V>
Range<T> MakeRange::to(T start, U end, V step) {
    assert(step >= V{0});
    assert(start <= end);
    return Range{start, end + 1, step};
}

template<typename T, typename U, typename V>
Range<T> MakeRange::downTo(T start, U end, V step) {
    assert(step >= V{0});
    assert(start >= end);
    return Range{start, end - 1, -step};
}

template<typename T, typename U, typename V>
Range<T> MakeRange::downUntil(T start, U end, V step) {
    assert(step >= V{0});
    assert(start >= end);
    return Range{start, end, -step};
}

template<typename T, typename U, typename V, typename ValueType>
Range<ValueType> MakeRange::range(T start, U end, V step) {
    assert(static_cast<ValueType>(step) != static_cast<ValueType>(V{0}));
    if (static_cast<ValueType>(step) < static_cast<ValueType>(V{0})) {
        assert(static_cast<ValueType>(start) >= static_cast<ValueType>(end));
        return Range<ValueType>{static_cast<ValueType>(start), static_cast<ValueType>(end - 1),
                                static_cast<ValueType>(step)};
    } else {
        assert(static_cast<ValueType>(start) <= static_cast<ValueType>(end));
        return Range<ValueType>{static_cast<ValueType>(start), static_cast<ValueType>(end + 1),
                                static_cast<ValueType>(step)};
    }
}

template<typename T>
Range<T> MakeRange::range(T start) {
    const auto end = T{0};
    if (start <= end) {
        return Range{end, start, -T{1}};
    } else {
        return Range{end, start, T{1}};
    }
}
