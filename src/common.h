#pragma once

#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <string>
#include <vector>

namespace mapreduce
{

inline std::string trim(const std::string &value)
{
    std::size_t begin = 0;
    while (begin < value.size() &&
           (value[begin] == ' ' || value[begin] == '\t' ||
            value[begin] == '\r' || value[begin] == '\n'))
    {
        ++begin;
    }

    std::size_t end = value.size();
    while (end > begin &&
           (value[end - 1] == ' ' || value[end - 1] == '\t' ||
            value[end - 1] == '\r' || value[end - 1] == '\n'))
    {
        --end;
    }

    return value.substr(begin, end - begin);
}

inline void stripBom(std::string &line)
{
    if (line.size() >= 3 &&
        static_cast<unsigned char>(line[0]) == 0xEF &&
        static_cast<unsigned char>(line[1]) == 0xBB &&
        static_cast<unsigned char>(line[2]) == 0xBF)
    {
        line.erase(0, 3);
    }
}

inline void stripCarriageReturn(std::string &line)
{
    if (!line.empty() && line.back() == '\r')
    {
        line.pop_back();
    }
}

inline std::vector<std::string> parseCsvLine(const std::string &line)
{
    std::vector<std::string> fields;
    std::string field;
    bool quoted = false;

    for (std::size_t i = 0; i < line.size(); ++i)
    {
        const char character = line[i];

        if (quoted)
        {
            if (character == '"')
            {
                if (i + 1 < line.size() && line[i + 1] == '"')
                {
                    field.push_back('"');
                    ++i;
                }
                else
                {
                    quoted = false;
                }
            }
            else
            {
                field.push_back(character);
            }
        }
        else if (character == '"' && field.empty())
        {
            quoted = true;
        }
        else if (character == ',')
        {
            fields.push_back(field);
            field.clear();
        }
        else
        {
            field.push_back(character);
        }
    }

    fields.push_back(field);
    return fields;
}

inline bool findPriceColumn(const std::vector<std::string> &fields,
                            std::size_t &column)
{
    for (std::size_t i = 0; i < fields.size(); ++i)
    {
        if (trim(fields[i]) == "price")
        {
            column = i;
            return true;
        }
    }
    return false;
}

inline bool isHeader(const std::vector<std::string> &fields)
{
    return !fields.empty() && trim(fields[0]) == "id";
}

inline bool parseNumber(const std::string &token, long double &value)
{
    std::string text = trim(token);
    if (text.empty() || text == "NULL" || text == "null")
    {
        return false;
    }

    if (text.front() == '$')
    {
        text.erase(0, 1);
    }

    std::string cleaned;
    cleaned.reserve(text.size());
    for (const char character : text)
    {
        if (character != ',' && character != ' ')
        {
            cleaned.push_back(character);
        }
    }

    if (cleaned.empty())
    {
        return false;
    }

    errno = 0;
    char *end = nullptr;
    const long double parsed = std::strtold(cleaned.c_str(), &end);
    if (errno != 0 || end == cleaned.c_str() || *end != '\0' ||
        !std::isfinite(parsed) || parsed < 0.0L)
    {
        return false;
    }

    value = parsed;
    return true;
}

inline bool parsePrice(const std::string &token, long double &price)
{
    return parseNumber(token, price);
}

inline std::vector<std::string> splitTabs(const std::string &line)
{
    std::vector<std::string> fields;
    std::size_t start = 0;

    while (true)
    {
        const std::size_t tab = line.find('\t', start);
        fields.push_back(line.substr(start, tab - start));
        if (tab == std::string::npos)
        {
            return fields;
        }
        start = tab + 1;
    }
}

}
