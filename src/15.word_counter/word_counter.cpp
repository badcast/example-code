#include <iostream>
#include <string>

using namespace std;

int counts(const std::string& chars)
{
    int cnt = 0;
    bool in_word = false;
    for(int x = 0; x < chars.length(); ++x)
    {
        if(isspace(static_cast<unsigned char>(chars[x])))
        {
            in_word = false;
        }else if (!in_word)
        {
            in_word = true;
            ++cnt;
        }
    }
    return cnt;
}

int main(int argn, const char**argv)
{
    string words;

    for(int i = 1; i < argn; ++i)
    {
        words+=argv[i];
        if(i+1 < argn)
            words += static_cast<char>(' ');
    }
    if(words.empty())
    {
        cout << "Enter text (word calculation): ";
        cin >> words;
    }
    cout << "Words: " << counts(words) << endl;
    return 0;
}
