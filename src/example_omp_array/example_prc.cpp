
#include <iostream>
#include <omp.h>
#include <vector>
#include <cmath>
#include <string.h>

int main(void)
{
    constexpr std::size_t size = 6999999999;
    void* ma = malloc(size);

//    int n;
//    std::cin >> n;
    omp_set_num_threads(8);


    while(1)
#pragma omp parallel for
    for (size_t x = 0; x < 8; ++x) {
        memset(ma,-1,size);
    }

    std::cout << "End program." << std::endl;
}
