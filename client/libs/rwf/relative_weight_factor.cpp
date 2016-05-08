#include "relative_weight_factor.hpp"

/* Debug MACRO */
//#define DEBUG_RWF
#ifdef DEBUG_RWF
#define D(call) (call)
#else
#define D(call)
#endif

template <class T>
rwf::rwf<T>::rwf (std::vector<T> vec, int head, int tail)
: _factors (vec)
, _steps (head - tail) {
}

template <class T> rwf::rwf<T>::~rwf () {
}

template <class T> std::vector<T> rwf::rwf<T>::get_relative_weight_factor () {
    std::vector<T> vec;
    if (_factors.size () == 1) {
        vec.push_back (_steps);
    } else {
        T sum          = _sum (_factors);
        T sum_inv      = _sum_inv (_factors);
        T step_per_fac = _steps / sum_inv;
        D (std::cout << "Sum: " << sum << "Sum inv: " << sum_inv
                     << "Steps/fac: " << step_per_fac << std::endl);
        for (auto i : _factors) {
            // Add: (sum-val)*(step per factor)
            vec.push_back ((sum - i) * (step_per_fac));
            D (std::cout << "Fac: " << i << " InvFac: " << sum - i << std::endl);
        }
    }
    return vec;
}

template <class T> T rwf::rwf<T>::_sum (std::vector<T> vec) {
    T sum = 0;
    for (auto i : vec) {
        sum += i;
    }
    D (std::cout << "Sum: " << sum << std::endl);
    return sum;
}

template <class T> T rwf::rwf<T>::_sum_inv (std::vector<T> vec) {
    T sum     = _sum (vec);
    T sum_inv = 0;
    for (auto i : vec) {
        sum_inv += sum - i;
    }
    D (std::cout << "Sum inv: " << sum_inv << std::endl);
    return sum_inv;
}

template <class T> void rwf::rwf<T>::set_head_tail (int head, int tail) {
    _steps = head - tail;
}

template <class T> void rwf::rwf<T>::set_factors (std::vector<T> vec) {
    _factors = vec;
}
