#pragma once

#include <array>
#include <cstddef>
#include <ctime>
#include <memory>
#include <string>
#include <vector>

#include <boost/asio.hpp>

namespace async
{

class Runtime;
class StaticCollector;

// Serves a single client. Commands that are not part of a dynamic block are
// forwarded to the shared StaticCollector, dynamic blocks stay private to this
// connection.
class Connection : public std::enable_shared_from_this<Connection>
{
public:
    Connection(boost::asio::ip::tcp::socket socket,
               StaticCollector &collector,
               Runtime &runtime);

    void start();

private:
    void readSome();
    void onRead(const boost::system::error_code &error, std::size_t length);
    void handleLine(const std::string &line);
    void emitDynamic();
    void finish();

    boost::asio::ip::tcp::socket socket_;
    StaticCollector &collector_;
    Runtime &runtime_;
    std::array<char, 4096> buffer_{};
    std::string pending_;
    std::vector<std::string> dynamic_;
    std::time_t dynamic_start_ = 0;
    int depth_ = 0;
    bool dynamic_mode_ = false;
};

} // namespace async
