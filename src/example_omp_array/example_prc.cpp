
#include <iostream>
#include <omp.h>
#include <vector>
#include <cmath>
#include <string.h>

int main(void)
{
    constexpr std::size_t size = 5999999999 + 1; // 6 GB
    void* ma = malloc(size);

    if (ma == nullptr)
        throw 0;

    int threads;

    std::cout << "set thread count: ";
    std::cin >> threads;

    omp_set_dynamic(0); // Explicitly disable dynamic teams
    omp_set_num_threads(threads = std::max(1, std::min(32, threads)));
    std::cout << "runned threads: " << threads << std::endl;
    std::ldiv_t thr = std::ldiv(size, threads);
#pragma omp parallel for
    for (size_t x = 0; x < threads; ++x) {
        threads = omp_get_num_threads();
        memset(ma + thr.quot * x, -1, thr.quot + thr.rem);
    }

    int* last = static_cast<int*>(ma + size - sizeof(int) * 2);

    free(ma);

    std::cout << "End program." << std::endl;
}
