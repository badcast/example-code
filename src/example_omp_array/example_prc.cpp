
#include <iostream>
#include <omp.h>
#include <vector>
#include <cmath>

int main(void)
{
    constexpr std::size_t size = 999999999;
    std::vector<double> a(size), b(size), c(size);

#pragma omp parallel for
    for (size_t x = 0; x < size; ++x) {
        a[x] = x * 2;
        b[x] = x * 10;
    }

    omp_set_num_threads(4);

#pragma omp parallel for
    for (size_t x = 0; x < size; ++x) {
        c[x] = std::sqrt(a[x]) * b[x];
    }

    std::cout << "End program." << std::endl;
}
