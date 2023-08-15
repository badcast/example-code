
#include <iostream>
#include <omp.h>
#include <vector>
#include <cmath>
#include <string.h>

#ifdef __unix__
#include <sys/types.h>
#include <sys/sysinfo.h>
#elif __WIN32__
#include <windows.h>
#endif
int main(void)
{
    constexpr int FLAG_VAL = ~0;

    std::size_t size;
#ifdef __unix__

    struct sysinfo __sysinfo;
    if (sysinfo(&__sysinfo) != 0) {
        std::cerr << "Fail: " << strerror(errno) << std::endl;
        return EXIT_FAILURE;
    }

    size = __sysinfo.freeram * __sysinfo.mem_unit;

#elif __WIN32__
    MEMORYSTATUS __meminfo;
    GlobalMemoryStatus(&__meminfo);

    size = __meminfo.dwAvailPhys;
#endif

    std::cout << "Free available memory: " << (size / 1024 / 1024) << " MiB" << std::endl;

    void* ma = malloc(size);

    if (ma == nullptr)
        throw 0;

    int threads = omp_get_max_threads();
    omp_set_dynamic(0); // Explicitly disable dynamic teams
    omp_set_num_threads(threads = std::max(1, std::min(32, threads)));
    std::cout << "runned threads: " << threads << std::endl;
    std::ldiv_t thr = std::ldiv(size, threads);

#pragma omp parallel for
    for (size_t x = 0; x < threads; ++x) {
        memset(ma + thr.quot * x, FLAG_VAL, thr.quot + thr.rem);
    }

    std::cout << "Press any key to close" << std::endl;
    getchar();
    free(ma);

    std::cout << "End program." << std::endl;
}
