#include "session.h"

#include <charconv>
#include <utility>
#include <vector>

#include "operations.h"

namespace join
{

namespace
{

bool parseId(const std::string &token, int &id)
{
    if (token.empty())
    {
        return false;
    }
    const char *begin = token.data();
    const char *end = begin + token.size();
    const auto result = std::from_chars(begin, end, id);
    return result.ec == std::errc() && result.ptr == end;
}

bool parseTable(const std::string &token, Database::Table &table)
{
    if (token == "A")
    {
        table = Database::Table::A;
        return true;
    }
    if (token == "B")
    {
        table = Database::Table::B;
        return true;
    }
    return false;
}



bool splitTokens(const std::string &line, std::vector<std::string> &tokens)
{
    std::size_t start = 0;
    while (true)
    {
        const std::size_t space = line.find(' ', start);
        const std::string token = line.substr(start, space - start);
        if (token.empty())
        {
            return false;
        }
        tokens.push_back(token);
        if (space == std::string::npos)
        {
            return true;
        }
        start = space + 1;
    }
}

bool isOperation(const std::string &line)
{
    return line == "INTERSECTION" || line == "SYMMETRIC_DIFFERENCE";
}

}

Session::Session(boost::asio::ip::tcp::socket socket,
                 Database &database,
                 boost::asio::thread_pool &workers,
                 boost::asio::io_context &io)
    : socket_(std::move(socket)),
      database_(database),
      workers_(workers),
      strand_(boost::asio::make_strand(io))
{
}

void Session::start()
{
    auto self = shared_from_this();
    boost::asio::post(strand_, [self] { self->readSome(); });
}

void Session::readSome()
{
    if (read_closed_ || closed_)
    {
        return;
    }

    auto self = shared_from_this();
    socket_.async_read_some(
        boost::asio::buffer(buffer_),
        boost::asio::bind_executor(
            strand_,
            [self](const boost::system::error_code &error, std::size_t length) {
                self->onRead(error, length);
            }));
}

void Session::onRead(const boost::system::error_code &error, std::size_t length)
{
    if (error)
    {
        if (!pending_.empty() && pending_.back() == '\r')
        {
            pending_.pop_back();
        }
        if (!pending_.empty())
        {
            commands_.push_back(std::move(pending_));
        }
        pending_.clear();
        read_closed_ = true;
        processNext();
        return;
    }

    pending_.append(buffer_.data(), length);

    std::size_t newline = std::string::npos;
    while ((newline = pending_.find('\n')) != std::string::npos)
    {
        std::string line = pending_.substr(0, newline);
        pending_.erase(0, newline + 1);
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }
        commands_.push_back(std::move(line));
    }

    processNext();
    readSome();
}

void Session::processNext()
{
    if (closed_ || processing_)
    {
        return;
    }

    if (commands_.empty())
    {
        if (read_closed_)
        {
            finish();
        }
        return;
    }

    std::string command = std::move(commands_.front());
    commands_.pop_front();

    if (isOperation(command))
    {
        beginOperation(command);
        return;
    }

    write(handleSimple(command));
}

std::string Session::handleSimple(const std::string &line)
{
    std::vector<std::string> tokens;
    if (!splitTokens(line, tokens) || tokens.empty())
    {
        return "ERR invalid command\n";
    }

    if (tokens[0] == "INSERT")
    {
        if (tokens.size() != 4)
        {
            return "ERR invalid command\n";
        }

        Database::Table table;
        if (!parseTable(tokens[1], table))
        {
            return "ERR unknown table\n";
        }

        int id = 0;
        if (!parseId(tokens[2], id))
        {
            return "ERR invalid id\n";
        }

        if (!database_.insert(table, id, tokens[3]))
        {
            return "ERR duplicate " + tokens[2] + "\n";
        }
        return "OK\n";
    }

    if (tokens[0] == "TRUNCATE")
    {
        if (tokens.size() != 2)
        {
            return "ERR invalid command\n";
        }

        Database::Table table;
        if (!parseTable(tokens[1], table))
        {
            return "ERR unknown table\n";
        }

        database_.truncate(table);
        return "OK\n";
    }

    return "ERR invalid command\n";
}

void Session::beginOperation(const std::string &command)
{
    processing_ = true;
    auto self = shared_from_this();

    boost::asio::post(
        workers_,
        [self, command] {
            std::string response;
            try
            {
                const Database::Snapshot snapshot = self->database_.snapshot();
                response = command == "INTERSECTION"
                               ? intersection(snapshot)
                               : symmetricDifference(snapshot);
            }
            catch (const std::exception &)
            {
                response = "ERR internal error\n";
            }

            boost::asio::post(
                self->strand_,
                [self, response = std::move(response)]() mutable {
                    self->processing_ = false;
                    self->write(std::move(response));
                });
        });
}

void Session::write(std::string response)
{
    if (closed_)
    {
        return;
    }

    write_buffer_ = std::move(response);
    auto self = shared_from_this();
    boost::asio::async_write(
        socket_,
        boost::asio::buffer(write_buffer_),
        boost::asio::bind_executor(
            strand_,
            [self](const boost::system::error_code &error, std::size_t length) {
                self->onWrite(error, length);
            }));
}

void Session::onWrite(const boost::system::error_code &error, std::size_t)
{
    if (error)
    {
        finish();
        return;
    }

    write_buffer_.clear();
    processNext();
}

void Session::finish()
{
    if (closed_)
    {
        return;
    }
    closed_ = true;

    boost::system::error_code ignored;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored);
    socket_.close(ignored);
}

}
