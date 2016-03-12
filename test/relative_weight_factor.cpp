#include "../libs/rwf/relative_weight_factor.hpp"
#include <iostream>
#include <stdio.h>

int main (int argc, char const* argv[]) {
    std::cout << "Testing RWF" << std::endl;
    relative_weight_factor::rwf<double> rwf_double ({ 1, 1, 1, 1, 1, 1, 1, 1 }, 100);
    std::cout << std::endl
              << rwf_double.get_relative_weight_factor ().at (0) << std::endl;
    rwf_double.set_factors ({ 2, 1, 1, 1, 1, 1, 1, 1 });
    std::cout << std::endl
              << rwf_double.get_relative_weight_factor ().at (0) << std::endl;
    rwf_double.set_head_tail (50);
    std::cout << std::endl
              << rwf_double.get_relative_weight_factor ().at (0) << std::endl;
    return 0;
}
