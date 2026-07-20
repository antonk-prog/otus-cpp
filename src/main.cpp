#include <iostream>
#include "matrix.h"
#include <tuple>

int main()
{
    Matrix matrix(0);

    for (int i = 0; i <= 9; ++i) matrix[i][i] = i;

    for (int i = 0; i <= 9; ++i) matrix[i][9 - i] = 9 - i;

    for (int r = 1; r <= 8; ++r)
    {
        for (int c = 1; c <= 8; ++c)
        {
            if (c != 1) std::cout << ' ';
            std::cout << matrix(r, c);
        }
        std::cout << '\n';
    }

    std::cout << matrix.size() << std::endl;

    for (auto t : matrix)
    {
        int x, y, v;
        std::tie(x, y, v) = t;
        std::cout << x << ' ' << y << ' ' << v << '\n';
    }

    return 0;
}