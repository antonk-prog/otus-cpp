#include "ip_filter_lib.h"

#include <gtest/gtest.h>

using ip_filter::filter_any;
using ip_filter::filter;
using ip_filter::ip_t;
using ip_filter::parse_ip;
using ip_filter::split;

TEST(Split, KeepsEmptyParts)
{
	EXPECT_EQ(split("..", '.'), std::vector<std::string>({"", "", ""}));
}

TEST(ParseIp, ParsesValidIp)
{
	ip_t ip{};

	EXPECT_TRUE(parse_ip("46.70.29.76", ip));
	EXPECT_EQ(ip, (ip_t{46, 70, 29, 76}));
}

TEST(ParseIp, RejectsInvalidIp)
{
	ip_t ip{};

	EXPECT_THROW(parse_ip("46.70.29", ip), std::invalid_argument);
	EXPECT_THROW(parse_ip("46.70.29.256", ip), std::invalid_argument);
}

TEST(Filter, SelectsExpectedIps)
{
	const std::vector<ip_t> pool{
		ip_t{222, 173, 235, 246},
		ip_t{1, 70, 44, 170},
		ip_t{46, 70, 29, 76},
		ip_t{39, 46, 86, 85},
	};

	EXPECT_EQ(filter(pool, 1), std::vector<ip_t>({ip_t{1, 70, 44, 170}}));
	EXPECT_EQ(filter(pool, 46, 70), std::vector<ip_t>({ip_t{46, 70, 29, 76}}));
	EXPECT_EQ(filter_any(pool, 46), std::vector<ip_t>({ip_t{46, 70, 29, 76}, ip_t{39, 46, 86, 85}}));
}
