#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char *argv[])
{
    vector<int> n {31, 25, 32, 43, 59, 100};

    for(int i = 1; i < n.size() - 1; ++i)
    {
        int x = n[i];
        int j = i;

        while(j > 0 && n[j - 1] > x)
        {
            n[j] = n[j - 1];
            j = j - 1;
        }
        n[j] = x;
    }

    int pp = 0;

    return 0;
}
