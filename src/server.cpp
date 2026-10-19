#include "server.h"

#include <memory>
#include <utility>

#include <boost/asio/ip/v6_only.hpp>

#include "session.h"

namespace join
{

Server::Server(boost::asio::io_context &io,
               boost::asio::thread_pool &workers,
               std::uint16_t port)
    : io_(io), workers_(workers), acceptor_(io)
{
    openAcceptor(port);
    acceptor_.listen();
}

void Server::openAcceptor(std::uint16_t port)
{
    boost::system::error_code error;



    const boost::asio::ip::tcp::endpoint v6_endpoint(boost::asio::ip::tcp::v6(), port);
    acceptor_.open(v6_endpoint.protocol(), error);
    if (!error)
    {
        acceptor_.set_option(boost::asio::ip::v6_only(false), error);
        error.clear();
        acceptor_.set_option(boost::asio::socket_base::reuse_address(true), error);
        error.clear();
        acceptor_.bind(v6_endpoint, error);
        if (!error)
        {
            return;
        }
        acceptor_.close();
    }

    const boost::asio::ip::tcp::endpoint v4_endpoint(boost::asio::ip::tcp::v4(), port);
    acceptor_.open(v4_endpoint.protocol(), error);
    if (error)
    {
        throw boost::system::system_error(error);
    }
    acceptor_.set_option(boost::asio::socket_base::reuse_address(true), error);
    acceptor_.bind(v4_endpoint, error);
    if (error)
    {
        throw boost::system::system_error(error);
    }
}

void Server::start()
{
    accept();
}

void Server::accept()
{
    acceptor_.async_accept(
        [this](const boost::system::error_code &error,
               boost::asio::ip::tcp::socket socket) {
            if (!error)
            {
                std::make_shared<Session>(std::move(socket), database_,
                                          workers_, io_)
                    ->start();
            }
            if (acceptor_.is_open())
            {
                accept();
            }
        });
}

}
