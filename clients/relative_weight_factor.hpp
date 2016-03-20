#ifndef relative_weight_factor_HPP
#define relative_weight_factor_HPP

#include <cstring>
#include <iostream>
#include <stdio.h>
#include <vector>

namespace relative_weight_factor {
template <class T> class rwf {
    private:
    /* data */
    std::vector<T> _factors;
    int _steps;
    T _sum (std::vector<T> vec);
    T _sum_inv (std::vector<T> vec);

    public:
    rwf (std::vector<T> vec, int head = 100, int tail = 0);
    ~rwf ();
    std::vector<T> get_relative_weight_factor ();
    void set_head_tail (int head = 100, int tail = 0);
    void set_factors (std::vector<T> vec);
};
} /* relative_weight_factor */

/* explicit instantiation declaration*/
template class relative_weight_factor::rwf<int>;
template class relative_weight_factor::rwf<double>;
template class relative_weight_factor::rwf<long double>;
#endif // relative_weight_factor_HPP