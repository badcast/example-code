#include <iostream>
#include <vector>
#include <cstdint>
#include <cmath>
#include <tuple>
#include <random>
#include <type_traits>

using namespace std;

template <typename T>
class ComputeUnit
{
    static_assert(!std::is_signed<T>::value, "T must be an unsigned type");

public:
    using type = T;
    int max_range()
    {
        return sizeof(type) * 8 - 1;
    }
    void reverse(vector<T> &vec)
    {
        for(int x = 0, y = vec.size() - 1; x < y; ++x, --y)
        {
            T tmp = vec[x];
            vec[x] = vec[y];
            vec[y] = tmp;
        }
    }

    void shuffle(vector<T> &vec)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
#pragma omp parallel for
        for(T x = 0; x < vec.size(); ++x)
        {
            std::uniform_int_distribution<T> dis(x, vec.size());
            T from = dis(gen);
            swap(vec[x], vec[from]);
        }
    }

    T bin_search(const vector<T> &vec, T value, T &score)
    {
        T x = 0;
        T y = vec.size() - 1;
        T m;

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

    void fill_arithmetic_progression(vector<T> &vec)
    {
        T start = 1;
        int difference_progression = 3;
        for(T x = 0; x < vec.size(); ++x)
        {
            vec[x] = start;
            start += difference_progression;
        }
    }

    T compute(uint32_t ranges)
    {
        T test;
        T score = 0;
        vector<T> vec(ranges);

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

        shuffle(vec);

        return score;
    }
};

int main(int argc, const char *argv[])
{
    using type = unsigned int;
    ComputeUnit<type> unit;
    int max_range = unit.max_range();
    vector<tuple<type, type>> calculated_scores(max_range);

#pragma omp parallel for
    for(int x = 0; x < max_range; ++x)
    {
        type compute_value = static_cast<type>(powl(2, x));
        type score = unit.compute(compute_value);
        calculated_scores[x] = std::move(make_tuple(compute_value, score));
    }

    // Print results
    for(auto &elem : calculated_scores)
    {
        cout << "compute score for [" << get<0>(elem) << "] = " << get<1>(elem) << endl;
    }
}
