
#include <iostream>
#include <omp.h>
#include <vector>

int main(void)
{
    constexpr std::size_t size = 99999999;
    std::vector<int> a(size), b(size), c(size);


    for (int x = 0; x < size; ++x) {
        a[x] = x * 2;
        b[x] = x * 10;
    }


#pragma omp parallel for
    for (int x = 0; x < size; ++x) {
        c[x] = a[x] + b[x];
    }

    std::cout << "End program." << std::endl;
}
