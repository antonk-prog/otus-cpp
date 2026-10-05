#pragma once

#include <ctime>
#include <string>
#include <vector>

namespace async
{

struct Block
{
    std::vector<std::string> commands;
    std::time_t timestamp = 0;
};

} // namespace async
