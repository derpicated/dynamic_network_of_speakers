#include "relative_weight_factor.hpp"
#include <iostream>
#include <stdio.h>
#include <vector>

template <class T>
relative_weight_factor<T>::relative_weight_factor (std::vector<T> vec, int head, int tail) {
    T sum_of_elements = 0;
    for (auto i : vec) {
        sum_of_elements += i;
        std::cout << i << " ";
    }
    std::cout << std::endl << "Total: " << sum_of_elements << std::endl;
}
template relative_weight_factor<int>::relative_weight_factor (std::vector<int> vec, int head, int tail);
template relative_weight_factor<double>::relative_weight_factor (std::vector<double> vec, int head, int tail);

template <class T> relative_weight_factor<T>::~relative_weight_factor () {
}
template relative_weight_factor<int>::~relative_weight_factor ();
template relative_weight_factor<double>::~relative_weight_factor ();
