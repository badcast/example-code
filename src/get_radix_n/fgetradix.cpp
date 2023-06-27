#include <iostream>
#include <cstdint>
#include <cmath>
#include <stdint.h>
#include <limits.h>

// get last radix (n)
std::uint64_t fget_lastof(std::uint64_t number, int a)
{
    std::uint64_t x = std::pow(10, a);
    auto d = std::ldiv(number, x);
    if(number < x/10)
        d.rem = 0;
    return d.rem;
}

int main()
{
    int number = INT32_MAX;

    std::cout << number << std::endl;
    int _catch, i = 1;
    while ((_catch = fget_lastof(number, i)) != 0) {
        std::cout << "\t [" << i << "] " << _catch << std::endl;
        ++i;
    }
    std::cout << "End program" << std::endl;

    return 0;
}
