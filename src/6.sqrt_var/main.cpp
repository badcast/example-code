#include <iostream>
#include <cmath>

#if __WIN32__
#include <windows.h>
#endif

using namespace std;

int dt(int&& a, int&& b, int&& c) { return (b * b) - 4 * a * c; }

int main()
{
#ifdef _WINDOWS_
    // for russian Codepage
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
#endif
    int a, b, c, d;
    cout << "Вычисление квадратного корня" << endl;

    cout << "Введите, a, b, c: ";
    cin >> a >> b >> c;
    cout << "Дискреминант = " << (d = dt(std::move(a), std::move(b), std::move(c))) << endl;

    cout << "Результат: " << endl;
    if (d > 0) {
        cout << "\tx₁ = " << (0.5 * (-b - std::sqrt(d)) / a) << endl;
        cout << "\tx₂ = " << (0.5 * (-b + std::sqrt(d)) / a) << endl;
    } else if (d == 0) {
        cout << "\tx = " << (-0.5 * b / a) << endl;
    } else {
        cout << "Нет корней!" << endl;
    }

    return 0;
}
