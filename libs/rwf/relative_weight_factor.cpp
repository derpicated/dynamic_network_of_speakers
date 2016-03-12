#include "relative_weight_factor.hpp"
#include <iostream>
#include <stdio.h>
#include <vector>

template <class T>
relative_weight_factor::rwf<T>::rwf (std::vector<T> vec, int head, int tail) {
    T sum_of_elements = 0;
    for (auto i : vec) {
        sum_of_elements += i;
        std::cout << i << " ";
    }
    std::cout << std::endl << "Total: " << sum_of_elements << std::endl;
}

template <class T> relative_weight_factor::rwf<T>::~rwf () {
}
