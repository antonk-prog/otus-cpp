#pragma once

#include <array>
#include <cstddef>
#include <deque>
#include <memory>
#include <string>

#include <boost/asio.hpp>

#include "database.h"

namespace join
{



class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(boost::asio::ip::tcp::socket socket,
            Database &database,
            boost::asio::thread_pool &workers,
            boost::asio::io_context &io);

    void start();

private:
    using Strand = boost::asio::strand<boost::asio::io_context::executor_type>;

    void readSome();
    void onRead(const boost::system::error_code &error, std::size_t length);

    void processNext();
    std::string handleSimple(const std::vector<std::string> &tokens);
    void beginOperation(const std::string &command);

    void write(std::string response);
    void onWrite(const boost::system::error_code &error, std::size_t length);
    void finish();

    boost::asio::ip::tcp::socket socket_;
    Database &database_;
    boost::asio::thread_pool &workers_;
    Strand strand_;
    std::array<char, 8192> buffer_{};
    std::string pending_;
    std::deque<std::string> commands_;
    std::string write_buffer_;
    bool processing_ = false;
    bool read_closed_ = false;
    bool closed_ = false;
};

}
