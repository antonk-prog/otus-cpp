#pragma once

#include <cstdint>

#include <boost/asio.hpp>

#include "database.h"

namespace join
{

class Server
{
public:
    Server(boost::asio::io_context &io,
           boost::asio::thread_pool &workers,
           std::uint16_t port);

    void start();

private:
    void accept();
    void openAcceptor(std::uint16_t port);

    boost::asio::io_context &io_;
    boost::asio::thread_pool &workers_;
    boost::asio::ip::tcp::acceptor acceptor_;
    Database database_;
};

}
