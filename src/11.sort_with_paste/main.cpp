#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char *argv[])
{
    vector<int> n {25, 25, 24, 43, 3, 2};

    for(int i = 1; i < n.size(); ++i)
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



    return 0;
}
