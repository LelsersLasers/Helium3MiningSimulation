#include <iostream>

#include "sample_lib.h"

int main() {
    int sample_sum = sample_lib::add(2, 3);
    std::cout << "Sample sum: " << sample_sum << std::endl;
    return 0;
}
