#pragma once

#include <ctime>
#include <string>
#include <vector>

struct Block
{
    std::vector<std::string> commands;
    std::time_t timestamp;
};

class IObserver
{
public:
    virtual ~IObserver() = default;

    virtual void update(const Block &block) = 0;
};