#include "file_writer.h"

#include <fstream>
#include <string>

void FileWriter::update(const Block &block)
{
    const std::string filename =
        "bulk" + std::to_string(static_cast<long long>(block.timestamp)) +
        "_" + std::to_string(block_counter_) + ".log";
    ++block_counter_;

    std::ofstream out(filename);
    for (const auto &command : block.commands)
    {
        out << command << '\n';
    }
}