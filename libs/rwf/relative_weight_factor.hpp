#ifndef relative_weight_factor_HPP
#define relative_weight_factor_HPP

#include <cstring>
#include <vector>

template <class T> class relative_weight_factor {
    private:
    /* data */
    public:
    relative_weight_factor (std::vector<T> vec, int head = 0, int tail = 100);
    ~relative_weight_factor ();
};

#endif // relative_weight_factor_HPP
