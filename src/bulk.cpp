#include "bulk.h"

#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace
{

    struct Block
    {
        std::vector<std::string> commands;
        std::time_t timestamp;
    };

    void print_console(const Block &block)
    {
        std::cout << "bulk: ";
        for (size_t i = 0; i < block.commands.size(); ++i)
        {
            if (i != 0)
            {
                std::cout << ", ";
            }
            std::cout << block.commands[i];
        }
        std::cout << '\n';
    }

    void save_file(const Block &block)
    {
        const std::string filename =
            "bulk" + std::to_string(static_cast<long long>(block.timestamp)) + ".log";

        std::ofstream out(filename);
        for (const auto &command : block.commands)
        {
            out << command << '\n';
        }
    }

} // namespace

int bulk(size_t block_size)
{
    std::vector<std::string> current;
    std::time_t start = 0;
    bool dynamic = false;
    int depth = 0;

    auto emit = [&]()
    {
        if (current.empty())
        {
            return;
        }
        Block block{current, start};
        print_console(block);
        save_file(block);
        current.clear();
    };

    std::string line;
    while (std::getline(std::cin, line))
    {
        if (!dynamic)
        {
            if (line == "{")
            {
                emit();
                dynamic = true;
                depth = 1;
                continue;
            }
            if (current.empty())
            {
                start = std::time(nullptr);
            }
            current.push_back(line);
            if (current.size() == block_size)
            {
                emit();
            }
        }
        else
        {
            if (line == "{")
            {
                ++depth;
            }
            else if (line == "}")
            {
                --depth;
                if (depth == 0)
                {
                    emit();
                    dynamic = false;
                }
            }
            else
            {
                if (current.empty())
                {
                    start = std::time(nullptr);
                }
                current.push_back(line);
            }
        }
    }

    if (dynamic)
    {
        return 0;
    }
    emit();
    return 0;
}