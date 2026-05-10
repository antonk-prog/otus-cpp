#include "ip_filter_lib.h"

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

    bool compare_desc(std::vector<std::string> lhs, std::vector<std::string> rhs)
    {
        auto lhs_begin = lhs.cbegin();
        auto rhs_begin = rhs.cbegin();
        while (lhs_begin != lhs.cend() && rhs_begin != rhs.cend())
        {
            if (std::stoi(*lhs_begin) > std::stoi(*rhs_begin))
            {
                return true;
            }
            else if (*lhs_begin == *rhs_begin)
            {
                lhs_begin++;
                rhs_begin++;
            }
            else
            {
                return false;
            }
        }
        return false;
    }
}
