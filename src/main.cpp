#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <thread>

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include "server.h"

namespace
{

bool parsePort(const char *arg, unsigned long &port)
{
    if (arg == nullptr || *arg == '\0')
    {
        return false;
    }
    char *end = nullptr;
    const unsigned long parsed = std::strtoul(arg, &end, 10);
    if (*end != '\0' || parsed == 0 || parsed > 65535)
    {
        return false;
    }
    port = parsed;
    return true;
}

}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: join_server <port>\n";
        return 1;
    }

    unsigned long port = 0;
    if (!parsePort(argv[1], port))
    {
        std::cerr << "Port must be an integer in range 1..65535!\n";
        return 1;
    }

    try
    {
        boost::asio::io_context io;
        const unsigned hardware = std::thread::hardware_concurrency();
        boost::asio::thread_pool workers(std::max(2u, hardware));

        boost::asio::signal_set signals(io, SIGINT, SIGTERM);
        signals.async_wait(
            [&io](const boost::system::error_code &, int) { io.stop(); });

        join::Server server(io, workers, static_cast<std::uint16_t>(port));
        server.start();

        io.run();
        workers.join();
    }
    catch (const std::exception &error)
    {
        std::cerr << "Server error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
