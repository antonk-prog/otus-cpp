#include "ip_filter_lib.h"

#include <algorithm>
#include <stdexcept>

// ("",  '.') -> [""]
// ("11", '.') -> ["11"]
// ("..", '.') -> ["", "", ""]
// ("11.", '.') -> ["11", ""]
// (".11", '.') -> ["", "11"]
// ("11.22", '.') -> ["11", "22"]
namespace ip_filter
{
    std::vector<std::string> split(const std::string &str, char d)
    {
        std::vector<std::string> r;

        std::string::size_type start = 0;
        std::string::size_type stop = str.find_first_of(d);
        while (stop != std::string::npos)
        {
            r.push_back(str.substr(start, stop - start));

            start = stop + 1;
            stop = str.find_first_of(d, start);
        }

        r.push_back(str.substr(start));

        return r;
    }

    bool parse_ip(const std::string &line, ip_t &ip)
    {
        const auto parts = split(line, '.');
        if (parts.size() != ip.size())
        {
            throw std::invalid_argument("invalid ip address format");
        }

        for (std::size_t index = 0; index < ip.size(); ++index)
        {
            const auto &part = parts[index];
            if (part.empty())
            {
                throw std::invalid_argument("invalid ip address format");
            }

            std::size_t parsed = 0;
            const int value = std::stoi(part, &parsed);
            if (parsed != part.size() || value < 0 || value > 255)
            {
                throw std::invalid_argument("invalid ip address format");
            }

            ip[index] = value;
        }

        return true;
    }

    bool compare_desc(const ip_t &lhs, const ip_t &rhs)
    {
        for (std::size_t index = 0; index < lhs.size(); ++index)
        {
            if (lhs[index] != rhs[index])
            {
                return lhs[index] > rhs[index];
            }
        }

        return false;
    }

    std::vector<ip_t> filter_first(const std::vector<ip_t> &ip_pool, int first_octet)
    {
        std::vector<ip_t> result;
        std::copy_if(ip_pool.begin(), ip_pool.end(), std::back_inserter(result),
            [first_octet](const ip_t &ip)
            {
                return ip[0] == first_octet;
            });
        return result;
    }

    std::vector<ip_t> filter_first_second(const std::vector<ip_t> &ip_pool, int first_octet, int second_octet)
    {
        std::vector<ip_t> result;
        std::copy_if(ip_pool.begin(), ip_pool.end(), std::back_inserter(result),
            [first_octet, second_octet](const ip_t &ip)
            {
                return ip[0] == first_octet && ip[1] == second_octet;
            });
        return result;
    }

    std::vector<ip_t> filter_any(const std::vector<ip_t> &ip_pool, int octet)
    {
        std::vector<ip_t> result;
        std::copy_if(ip_pool.begin(), ip_pool.end(), std::back_inserter(result),
            [octet](const ip_t &ip)
            {
                return std::any_of(ip.begin(), ip.end(), [octet](int value)
                {
                    return value == octet;
                });
            });
        return result;
    }
}
