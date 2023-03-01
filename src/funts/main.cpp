#include <cstdlib>
#include <iostream>
#include <string>
int main()
{
    using namespace std;
    int a;
    double b;
    string accept;
    cout << "Выберите исходную массу: ";
    cin >> a;
    cout << "Фунты? (yes/no): ";
    cin >> accept;
    accept[0]=tolower(accept[0]);

    cout << "В ";
    if(accept.front() == 'y'){
        b = a * 0.45359;
        cout << "килогораммах";
    }
    else {
        b = a / 0.45359;
        cout << "фунтах";
    }

    cout << " будет = " << b << endl;
    cout << "Программа завершена." << endl;
}
