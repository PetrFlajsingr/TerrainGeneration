//
// Created by Petr on 18.11.2019.
//

#ifndef UTILITIES_RANGE_H
#define UTILITIES_RANGE_H

#include <cassert>
#include <meta/meta.h>

template<typename T>
class Range {
private:
    struct iterator {
        iterator() = default;

        iterator(T value, T step, T max, bool up);

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
        T endValue;
        bool up;
    };

public:
    template<typename U = T, typename V = T>
    Range(T start, U end, V step = V{1});

    iterator begin();

    iterator end();

private:
    const T _start;
    const T _end;
    const T _step;
};

namespace MakeRange {
    template<typename T, typename U = T, typename V = T>
    Range<T> until(T start, U end, V step = V{1});

    template<typename T, typename U = T, typename V = T>
    Range<T> to(T start, U end, V step = V{1});

    template<typename T, typename U = T, typename V = T>
    Range<T> downTo(T start, U end, V step = V{1});

    template<typename T, typename U = T, typename V = T>
    Range<T> downUntil(T start, U end, V step = V{1});

    template<typename T, typename U, typename V,
            typename ValueType = typename std::conditional_t<any_of_v<std::is_floating_point, T, U, V>, highest_precision_t<T, U, V>,
                    std::conditional_t<any_of_v<std::is_signed, T, U, V>, largest_signed_integer_t<T, U, V>,
                            largest_integer_t<T, U, V>>>>
    Range<ValueType> range(T start, U end, V step);

    template<typename T>
    Range<T> range(T start);
} // namespace MakeRange

#include "Range.tpp"

#endif // UTILITIES_RANGE_H
