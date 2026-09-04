#include <cstdlib>
#include <iostream>
#include <string>

#include "bulk.h"

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
        std::cout << "Block size must be   positive integer!\n";
        return 1;
    }

    return bulk(block_size);
}