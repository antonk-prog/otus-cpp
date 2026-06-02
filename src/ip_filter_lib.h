#pragma once
#include <array>
#include <vector>
#include <string>
#include <algorithm>
namespace ip_filter
{
    using ip_t = std::array<int, 4>;

    std::vector<std::string> split(const std::string &str, char d);
    bool parse_ip(const std::string &line, ip_t &ip);
    template<typename... Octets>
    std::vector<ip_t> filter(const std::vector<ip_t>& ip_pool, Octets... octets) {
        std::vector<ip_t> result;
        std::copy_if(ip_pool.begin(), ip_pool.end(), std::back_inserter(result),
            [octets...](const ip_t& ip) {
                size_t i = 0;
                bool match = true;
                ((match = match && (ip[i++] == octets)), ...);
                return match;
            });
        return result;
    }
    std::vector<ip_t> filter_any(const std::vector<ip_t> &ip_pool, int octet);
}
