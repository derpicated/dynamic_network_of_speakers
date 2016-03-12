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
    public:
    rwf (std::vector<T> vec, int head = 0, int tail = 100);
    ~rwf ();
};
} /* relative_weight_factor */

/* explicit instantiation declaration*/
template class relative_weight_factor::rwf<int>;
template class relative_weight_factor::rwf<double>;

#endif // relative_weight_factor_HPP
