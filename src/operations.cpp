 #include "operations.h"

#include <cstddef>

namespace join
{

namespace
{

std::size_t countDigits(int id)
{
    std::size_t digits = id <= 0 ? 1 : 0;
    for (int value = id; value != 0; value /= 10)
    {
        ++digits;
    }
    return digits;
}

std::size_t capacityFor(const Database::TableMap &table)
{
    std::size_t total = 0;
    for (const auto &entry : table)
    {
        total += countDigits(entry.first) + entry.second.size() + 3;
    }
    return total;
}

std::size_t estimateCapacity(const Database::TableMap &a, const Database::TableMap &b)
{
    return capacityFor(a) + capacityFor(b) + sizeof("OK");
}

void appendRow(std::string &out,
               int id,
               const std::string *a,
               const std::string *b)
{
    out += std::to_string(id);
    out += ',';
    if (a != nullptr)
    {
        out += *a;
    }
    out += ',';
    if (b != nullptr)
    {
        out += *b;
    }
    out += '\n';
}

}

std::string intersection(const Database::TableMap &a, const Database::TableMap &b)
{
    std::string out;
    out.reserve(estimateCapacity(a, b));
    auto left = a.begin();
    auto right = b.begin();

    while (left != a.end() && right != b.end())
    {
        if (left->first < right->first)
        {
            ++left;
        }
        else if (right->first < left->first)
        {
            ++right;
        }
        else
        {
            appendRow(out, left->first, &left->second, &right->second);
            ++left;
            ++right;
        }
    }

    out += "OK\n";
    return out;
}

std::string symmetricDifference(const Database::TableMap &a, const Database::TableMap &b)
{
    std::string out;
    out.reserve(estimateCapacity(a, b));
    auto left = a.begin();
    auto right = b.begin();

    while (left != a.end() && right != b.end())
    {
        if (left->first < right->first)
        {
            appendRow(out, left->first, &left->second, nullptr);
            ++left;
        }
        else if (right->first < left->first)
        {
            appendRow(out, right->first, nullptr, &right->second);
            ++right;
        }
        else
        {
            ++left;
            ++right;
        }
    }

    for (; left != a.end(); ++left)
    {
        appendRow(out, left->first, &left->second, nullptr);
    }
    for (; right != b.end(); ++right)
    {
        appendRow(out, right->first, nullptr, &right->second);
    }

    out += "OK\n";
    return out;
}

}
