#include "connection.h"

#include <utility>

#include "block.h"
#include "collector.h"
#include "runtime.h"

namespace async
{

namespace
{

const std::string OPEN = "{";
const std::string CLOSE = "}";

// Upper bound for a single line that has not been terminated by '\n' yet.
// Without it a client that never sends a newline could exhaust server memory.
constexpr std::size_t MAX_PENDING_LINE = 1u << 20; // 1 MiB

} // namespace

Connection::Connection(boost::asio::ip::tcp::socket socket,
                       StaticCollector &collector,
                       Runtime &runtime)
    : socket_(std::move(socket)), collector_(collector), runtime_(runtime)
{
    collector_.connectionOpened();
}

void Connection::start()
{
    readSome();
}

void Connection::readSome()
{
    auto self = shared_from_this();
    socket_.async_read_some(
        boost::asio::buffer(buffer_),
        [self](const boost::system::error_code &error, std::size_t length) {
            self->onRead(error, length);
        });
}

void Connection::onRead(const boost::system::error_code &error, std::size_t length)
{
    if (error)
    {
        // Process the last line even if the client closed the connection
        // without a trailing newline, then release the connection.
        if (!pending_.empty())
        {
            handleLine(pending_);
            pending_.clear();
        }
        finish();
        return;
    }

    pending_.append(buffer_.data(), length);

    // Process every complete line in a single pass and drop the consumed
    // prefix only once, keeping the whole operation O(N).
    std::size_t line_start = 0;
    std::size_t newline = pending_.find('\n', line_start);
    while (newline != std::string::npos)
    {
        handleLine(pending_.substr(line_start, newline - line_start));
        line_start = newline + 1;
        newline = pending_.find('\n', line_start);
    }
    if (line_start > 0)
    {
        pending_.erase(0, line_start);
    }

    if (pending_.size() > MAX_PENDING_LINE)
    {
        // The client keeps sending an unterminated line: drop its data and the
        // connection instead of growing the buffer without bound.
        pending_.clear();
        close();
        return;
    }

    readSome();
}

void Connection::handleLine(const std::string &line)
{
    if (!dynamic_mode_)
    {
        if (line == OPEN)
        {
            collector_.flush();
            dynamic_mode_ = true;
            depth_ = 1;
        }
        else if (line == CLOSE)
        {
            // A closing brace without an opening one is ignored.
        }
        else
        {
            collector_.add(line);
        }
        return;
    }

    if (line == OPEN)
    {
        ++depth_;
    }
    else if (line == CLOSE)
    {
        --depth_;
        if (depth_ == 0)
        {
            emitDynamic();
            dynamic_mode_ = false;
        }
    }
    else
    {
        if (dynamic_.empty())
        {
            dynamic_start_ = std::time(nullptr);
        }
        dynamic_.push_back(line);
    }
}

void Connection::emitDynamic()
{
    if (dynamic_.empty())
    {
        return;
    }
    runtime_.submit(Block{dynamic_, dynamic_start_});
    dynamic_.clear();
}

void Connection::close()
{
    boost::system::error_code ignored;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored);
    socket_.close(ignored);
}

void Connection::finish()
{
    collector_.connectionClosed();
}

} // namespace async
