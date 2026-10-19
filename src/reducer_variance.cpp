#include <iomanip>
#include <iostream>
#include <limits>
#include <string>

#include "common.h"

namespace
{

struct Accumulator
{
    long double count = 0.0L;
    long double sum = 0.0L;
    long double sumOfSquares = 0.0L;
};

}

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string currentKey;
    Accumulator accumulator;
    bool haveKey = false;

    const auto flush = [&] {
        if (!haveKey || accumulator.count <= 0.0L)
        {
            return;
        }

        const long double mean = accumulator.sum / accumulator.count;
        long double variance =
            accumulator.sumOfSquares / accumulator.count - mean * mean;
        if (variance < 0.0L)
        {
            variance = 0.0L;
        }

        std::cout << currentKey << '\t'
                  << std::setprecision(std::numeric_limits<double>::max_digits10)
                  << static_cast<double>(variance) << '\n';
    };

    std::string line;
    while (std::getline(std::cin, line))
    {
        mapreduce::stripCarriageReturn(line);
        if (line.empty())
        {
            continue;
        }

        const std::vector<std::string> fields = mapreduce::splitTabs(line);
        if (fields.size() != 4)
        {
            continue;
        }

        long double count = 0.0L;
        long double value = 0.0L;
        long double square = 0.0L;
        if (!mapreduce::parseNumber(fields[1], count) ||
            !mapreduce::parseNumber(fields[2], value) ||
            !mapreduce::parseNumber(fields[3], square) || count <= 0.0L)
        {
            continue;
        }

        if (!haveKey)
        {
            currentKey = fields[0];
            haveKey = true;
        }
        else if (fields[0] != currentKey)
        {
            flush();
            currentKey = fields[0];
            accumulator = Accumulator{};
        }

        accumulator.count += count;
        accumulator.sum += value;
        accumulator.sumOfSquares += square;
    }

    flush();
    return 0;
}
