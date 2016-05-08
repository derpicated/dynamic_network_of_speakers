#include "../client/libs/rwf/relative_weight_factor.hpp"
#include <iostream>
#include <stdio.h>
#include <vector>

int main () {
    std::cout << "Testing RWF" << std::endl;
    // answer is 12.5
    // rounding error incoming at whole numbers!
    rwf::rwf<int> rwf_int ({ 1, 1, 1, 1, 1, 1, 1 }, 100);
    rwf::rwf<float> rwf_float ({ 1, 1, 1, 1, 1, 1, 1, 1 }, 100);
    rwf::rwf<double> rwf_double ({ 1, 1, 1, 1, 1, 1, 1, 1 }, 100);
    rwf::rwf<long double> rwf_l_double ({ 1, 1, 1, 1, 1, 1, 1, 1 }, 100);
    rwf::rwf<float> single_value ({ 165.59 });


    std::cout << "int: " << rwf_int.get_relative_weight_factor ().at (0) << std::endl;
    std::cout << "float: " << rwf_float.get_relative_weight_factor ().at (0) << std::endl;
    std::cout << "double: " << rwf_double.get_relative_weight_factor ().at (0) << std::endl;
    std::cout << "long double: " << rwf_l_double.get_relative_weight_factor ().at (0)
              << std::endl;
    std::cout << "float_test: " << single_value.get_relative_weight_factor ().back ()
              << std::endl;

    return 0;
}
