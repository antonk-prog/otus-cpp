#include <cstdlib>
#include <iostream>
#include <string>

#include "async.h"

namespace
{

bool parse_bulk_size(const char *arg, std::size_t &bulk)
{
    if (arg == nullptr || *arg == '\0')
    {
        return false;
    }
    char *end = nullptr;
    const unsigned long value = std::strtoul(arg, &end, 10);
    if (*end != '\0' || value == 0)
    {
        return false;
    }
    bulk = static_cast<std::size_t>(value);
    return true;
}

} // namespace

int main(int argc, char **argv)
{
    std::size_t bulk = 5;
    if (argc > 1 && !parse_bulk_size(argv[1], bulk))
    {
        std::cerr << "Block size must be a positive integer!\n";
        return 1;
    }

    const async::handle_t handle = async::connect(bulk);

    std::string line;
    while (std::getline(std::cin, line))
    {
        line.push_back('\n');
        async::receive(handle, line.data(), line.size());
    }

    async::disconnect(handle);
    return 0;
}
