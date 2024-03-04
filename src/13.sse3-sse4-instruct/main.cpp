#include <iostream>
#include <emmintrin.h>
#include <smmintrin.h>

using namespace std;

int main()
{
    constexpr int N = 4;
    float a[N] {1.0f, 2.0f, 3.0f, 4.0f};
    float b[N] {5.0f, 6.0f, 7.0f, 8.0f};
    float r[N];

    __m128 xmm_a = _mm_loadu_ps(a);
    __m128 xmm_b = _mm_loadu_ps(b);

    __m128 xmm_r = _mm_add_ss(xmm_a, xmm_b);

    _mm_storeu_ps(r, xmm_r);

    std::cout << "SSE3 addition result:" << std::endl;
    for(int x = 0; x < N; ++x)
    {
        std::cout << r[x] << " ";
    }
    std::cout << std::endl;


    return 0;
}
