#include "../libs/rwf/relative_weight_factor.hpp"
#include <iostream>
#include <stdio.h>

int main (int argc, char const* argv[]) {
    std::cout << "Testing" << std::endl << std::endl;
    relative_weight_factor::rwf<int> rwf_int ({ 1, 2, 3 });
    relative_weight_factor::rwf<double> rwf_double ({ 1.1, 2.2, 3.3 });
    return 0;
}
