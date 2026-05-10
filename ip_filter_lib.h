#pragma once
#include <vector>
#include <string>

namespace ip_filter
{
    std::vector<std::string> split(const std::string &str, char d);
    bool compare_desc(std::vector<std::string> lhs, std::vector<std::string> rhs);
}
