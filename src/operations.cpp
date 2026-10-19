#include "operations.h"

#include <map>

namespace join
{

namespace
{

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

std::string intersection(const Database::Snapshot &snapshot)
{
    std::string out;
    auto left = snapshot.a.begin();
    auto right = snapshot.b.begin();

    while (left != snapshot.a.end() && right != snapshot.b.end())
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

std::string symmetricDifference(const Database::Snapshot &snapshot)
{
    std::string out;
    auto left = snapshot.a.begin();
    auto right = snapshot.b.begin();

    while (left != snapshot.a.end() && right != snapshot.b.end())
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

    for (; left != snapshot.a.end(); ++left)
    {
        appendRow(out, left->first, &left->second, nullptr);
    }
    for (; right != snapshot.b.end(); ++right)
    {
        appendRow(out, right->first, nullptr, &right->second);
    }

    out += "OK\n";
    return out;
}

}
