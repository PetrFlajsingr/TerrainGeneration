//
// Created by Petr on 18.11.2019.
//

#ifndef UTILITIES_RANGE_H
#define UTILITIES_RANGE_H


template<typename T>
class Range {
private:
    struct iterator {
        iterator() = default;
        iterator(T value, T step, T max) : value(value), step(step), max(max) {}
        iterator(const iterator &other) {
            value = other.value;
        }
        iterator &operator=(const iterator &other) {
            value = other.value;
        }

        bool operator==(const iterator &rhs) const {
            return value == rhs.value;
        }

        bool operator!=(const iterator &rhs) const {
            return !(rhs == *this);
        }

        T operator*() const {
            return value;
        }

        T* operator->() {
            return &value;
        }

        iterator &operator++() {
            value += step;
            if (value > max) {
                value = max;
            }
            return *this;
        }

        iterator operator++(int) {
            iterator tmp{*this};
            operator++();
            return tmp;
        }

        T value;
        T step;
        T max;
    };

public:
    Range(T start, T end, T step = T{1}) : _start(start), _end(end), _step(step) {}

    iterator begin() {
        return iterator{_start, _step, _end};
    }

    iterator end() {
        return iterator{_end, _step, _end};
    }

private:
    const T _start;
    const T _end;
    const T _step;
};

namespace MakeRange {
    template<typename T, typename U = T, typename V = T>
    Range<T> until(T start, U end, V step = V{1}) {
        return Range{start, end, step};
    }

  template<typename T, typename U = T>
  Range<T> until(T end, U step = U{1}) {
    return Range{T{0}, end, step};
  }

    template<typename T>
    Range<T> range(T start, T end, T step = T{1}) {
        return Range{start, end + 1, step};
    }

    template<typename T>
    Range<T> downTo(T start, T end, T step = T{1}) {
        return Range{end, start, -step};
    }
}


#endif //UTILITIES_RANGE_H
