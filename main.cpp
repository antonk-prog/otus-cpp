#include "patch_lib.h"
#include "ip_filter_lib.h"
#include <iostream>
#include <algorithm>

namespace
{
	void print_ip(const std::vector<std::string> &ip)
	{
		bool first = true;
		for (const auto &ip_part : ip)
		{
			if (!first)
			{
				std::cout << ".";
			}
			first = false;
			std::cout << ip_part;
		}
		std::cout << "\n";
	}
	void print_ip_pool(const std::vector<std::vector<std::string>> &ip_pool)
	{
		for (const auto &ip : ip_pool)
		{
			print_ip(ip);
		}
	}
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char const *argv[])
{
	using namespace ip_filter;
	// std::cout << "build " << version() << std::endl;
    try
    {
        std::vector<std::vector<std::string> > ip_pool;

        for(std::string line; std::getline(std::cin, line);)
        {
            auto v = split(line, '\t');
            ip_pool.push_back(split(v.at(0), '.'));
        }

        // TODO reverse lexicographically sort

		std::sort(ip_pool.begin(), ip_pool.end(), compare_desc);

		print_ip_pool(ip_pool);

		for (const auto& ip : ip_pool)
		{
			if (ip[0] == "1")
			{
				print_ip(ip);
			}
		}

		for (const auto& ip: ip_pool)
		{
			if (ip[0] == "46" && ip[1] == "70")
			{
				print_ip(ip);
			}
		}
		
		for (const auto& ip: ip_pool)
		{
			if (ip[0] == "46" || ip[1] == "46" || ip[2] == "46" || ip[3] == "46")
			{
				print_ip(ip);
			}
		}

        // 222.173.235.246
        // 222.130.177.64
        // 222.82.198.61
        // ...
        // 1.70.44.170
        // 1.29.168.152
        // 1.1.234.8

        // TODO filter by first byte and output
        // ip = filter(1)

        // 1.231.69.33
        // 1.87.203.225
        // 1.70.44.170
        // 1.29.168.152
        // 1.1.234.8

        // TODO filter by first and second bytes and output
        // ip = filter(46, 70)

        // 46.70.225.39
        // 46.70.147.26
        // 46.70.113.73
        // 46.70.29.76

        // TODO filter by any byte and output
        // ip = filter_any(46)

        // 186.204.34.46
        // 186.46.222.194
        // 185.46.87.231
        // 185.46.86.132
        // 185.46.86.131
        // 185.46.86.131
        // 185.46.86.22
        // 185.46.85.204
        // 185.46.85.78
        // 68.46.218.208
        // 46.251.197.23
        // 46.223.254.56
        // 46.223.254.56
        // 46.182.19.219
        // 46.161.63.66
        // 46.161.61.51
        // 46.161.60.92
        // 46.161.60.35
        // 46.161.58.202
        // 46.161.56.241
        // 46.161.56.203
        // 46.161.56.174
        // 46.161.56.106
        // 46.161.56.106
        // 46.101.163.119
        // 46.101.127.145
        // 46.70.225.39
        // 46.70.147.26
        // 46.70.113.73
        // 46.70.29.76
        // 46.55.46.98
        // 46.49.43.85
        // 39.46.86.85
        // 5.189.203.46
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << "\n";
    }

    return 0;
}
