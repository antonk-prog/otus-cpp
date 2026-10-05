#pragma once

#include <cstddef>
#include <cstdint>

#include <boost/asio.hpp>

#include "collector.h"

namespace async
{

class Runtime;

class Server
{
public:
    Server(boost::asio::io_context &io, std::uint16_t port, std::size_t block_size);

    void start();

private:
    void accept();
    void openAcceptor(std::uint16_t port);

    boost::asio::io_context &io_;
    boost::asio::ip::tcp::acceptor acceptor_;
    StaticCollector collector_;
    Runtime &runtime_;
};

} // namespace async
