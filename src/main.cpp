#include <cstdlib>
#include <iostream>
#include <string>

#include "bulk_parser.h"
#include "console_writer.h"
#include "file_writer.h"

namespace {

bool parse_block_size(const char* arg, size_t& block_size) {
    if (arg == nullptr || *arg == '\0') {
        return false;
    }
    char* end = nullptr;
    const unsigned long value = std::strtoul(arg, &end, 10);
    if (*end != '\0' || value == 0) {
        return false;
    }
    block_size = static_cast<size_t>(value);
    return true;
}

}  // namespace

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Invalid input! Example: ./bulk 3\n";
        return 1;
    }

    size_t block_size = 0;
    if (!parse_block_size(argv[1], block_size)) {
        std::cout << "Block size must be a positive integer!\n";
        return 1;
    }

    BulkParser parser(block_size);

    ConsoleWriter console_writer;
    FileWriter file_writer;

    parser.attach(&console_writer);
    parser.attach(&file_writer);

    parser.run();

    return 0;
}