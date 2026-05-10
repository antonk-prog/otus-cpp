#pragma once
#include <array>
#include <vector>
#include <string>

namespace ip_filter
{
    using ip_t = std::array<int, 4>;

    std::vector<std::string> split(const std::string &str, char d);
    bool parse_ip(const std::string &line, ip_t &ip);
    bool compare_desc(const ip_t &lhs, const ip_t &rhs);
    std::vector<ip_t> filter_first(const std::vector<ip_t> &ip_pool, int first_octet);
    std::vector<ip_t> filter_first_second(const std::vector<ip_t> &ip_pool, int first_octet, int second_octet);
    std::vector<ip_t> filter_any(const std::vector<ip_t> &ip_pool, int octet);
}
