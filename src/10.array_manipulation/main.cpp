#include <iostream>
#include <vector>
#include <cstdint>
#include <cmath>
#include <tuple>

using namespace std;

void reverse(vector<uintmax_t> &vec)
{
    for(int x = 0, y = vec.size() - 1; x < y; ++x, --y)
    {
        uintmax_t tmp = vec[x];
        vec[x] = vec[y];
        vec[y] = tmp;
    }
}

int bin_search(const vector<uintmax_t> &vec, uintmax_t value, uintmax_t &score)
{
    uintmax_t x = 0;
    uintmax_t y = vec.size() - 1;
    uintmax_t m;

    while(x <= y)
    {
        score++;
        m = (x + y) / 2;
        if(vec[m] > value)
            y = m - 1;
        else if(vec[m] < value)
            x = m + 1;
        else
            return m;
    }
    return -1;
}

void fill_arithmetic_progression(vector<uintmax_t> &vec)
{
    uintmax_t start = 1;
    int difference_progression = 3;
    for(uintmax_t x = 0; x < vec.size(); ++x)
    {
        vec[x] = start;
        start += difference_progression;
    }
}

uintmax_t compute(uint32_t ranges)
{
    uintmax_t test;
    uintmax_t score = 0;
    vector<uintmax_t> vec(ranges);

    // fill
    fill_arithmetic_progression(vec);

    // reversing
    // top to back
    reverse(vec);
    // back to top
    reverse(vec);

    if(ranges > 1)
    {
        score += vec[vec.size() - 1];
        test = bin_search(vec, vec[0], score);
        test = bin_search(vec, vec[ranges / 2], score);
        test = bin_search(vec, vec[ranges - 1], score);
    }
    return score;
}

int main(int argc, const char *argv[])
{
    int max_range = 30;
    vector<tuple<uintmax_t, uintmax_t>> calculated_scores(max_range);

#pragma omp parallel for
    for(int x = 0; x < max_range; ++x)
    {
        uintmax_t compute_value = powl(2, x);
        uintmax_t score = compute(compute_value);
        calculated_scores[x] = std::move(make_tuple(compute_value, score));
    }

    // Print results
    for(auto &elem : calculated_scores)
    {
        cout << "compute score for [" << get<0>(elem) << "] = " << get<1>(elem) << endl;
    }
}
