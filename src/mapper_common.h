#pragma once

#include <iomanip>
#include <iostream>
#include <limits>
#include <string>

#include "common.h"

namespace mapreduce
{

inline bool csvRecordComplete(const std::string &record)
{
    bool quoted = false;

    for (std::size_t i = 0; i < record.size(); ++i)
    {
        if (record[i] != '"')
        {
            continue;
        }

        if (quoted && i + 1 < record.size() && record[i + 1] == '"')
        {
            ++i;
            continue;
        }

        quoted = !quoted;
    }

    return !quoted;
}

inline void emitPrice(const std::string &record,
                      std::size_t &priceColumn,
                      bool includeSquares)
{
    if (trim(record).empty())
    {
        return;
    }

    const std::vector<std::string> fields = parseCsvLine(record);
    if (isHeader(fields))
    {
        std::size_t column = 0;
        if (findPriceColumn(fields, column))
        {
            priceColumn = column;
        }
        return;
    }

    if (fields.size() <= priceColumn)
    {
        return;
    }

    long double price = 0.0L;
    if (!parsePrice(fields[priceColumn], price))
    {
        return;
    }

    std::cout << (includeSquares ? "variance" : "mean") << "\t1\t"
              << std::setprecision(
                     std::numeric_limits<long double>::max_digits10)
              << price;

    if (includeSquares)
    {
        std::cout << '\t' << price * price;
    }

    std::cout << '\n';
}

inline int runPriceMapper(bool includeSquares)
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t priceColumn = 9;
    std::string record;
    std::string line;

    while (std::getline(std::cin, line))
    {
        stripCarriageReturn(line);

        if (record.empty())
        {
            stripBom(line);
        }
        else
        {
            record.push_back('\n');
        }

        record += line;

        if (csvRecordComplete(record))
        {
            emitPrice(record, priceColumn, includeSquares);
            record.clear();
        }
    }

    if (!record.empty() && csvRecordComplete(record))
    {
        emitPrice(record, priceColumn, includeSquares);
    }

    return 0;
}

}
