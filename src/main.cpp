#include <cstdint>
#include <cstdlib>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include "server.h"

namespace
{

bool parseUnsigned(const char *arg, unsigned long &value)
{
    if (arg == nullptr || *arg == '\0')
    {
        return false;
    }
    char *end = nullptr;
    const unsigned long parsed = std::strtoul(arg, &end, 10);
    if (*end != '\0' || parsed == 0)
    {
        return false;
    }
    value = parsed;
    return true;
}

} // namespace

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage: bulk_server <port> <bulk_size>\n";
        return 1;
    }

    unsigned long port = 0;
    unsigned long block_size = 0;
    if (!parseUnsigned(argv[1], port) || port > 65535)
    {
        std::cerr << "Port must be an integer in range 1..65535!\n";
        return 1;
    }
    if (!parseUnsigned(argv[2], block_size))
    {
        std::cerr << "Block size must be a positive integer!\n";
        return 1;
    }

    try
    {
        boost::asio::io_context io;

        boost::asio::signal_set signals(io, SIGINT, SIGTERM);
        signals.async_wait(
            [&io](const boost::system::error_code &, int) { io.stop(); });

        async::Server server(io, static_cast<std::uint16_t>(port), block_size);
        server.start();

        io.run();
    }
    catch (const std::exception &error)
    {
        std::cerr << "Server error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
