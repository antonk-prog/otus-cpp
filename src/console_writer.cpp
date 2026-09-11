#include "console_writer.h"

#include <iostream>

void ConsoleWriter::update(const Block &block)
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