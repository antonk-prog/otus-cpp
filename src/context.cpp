#include "context.h"

#include "runtime.h"

namespace async
{

namespace
{

constexpr const char *OPEN = "{";
constexpr const char *CLOSE = "}";

} // namespace

Context::Context(std::size_t block_size, Runtime &runtime)
    : runtime_(runtime), block_size_(block_size)
{
}

void Context::feed(const char *data, std::size_t size)
{
    if (data == nullptr || size == 0)
    {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    pending_.append(data, size);

    std::size_t newline = std::string::npos;
    while ((newline = pending_.find('\n')) != std::string::npos)
    {
        std::string line = pending_.substr(0, newline);
        pending_.erase(0, newline + 1);
        processLine(line);
    }
}

void Context::finish()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!pending_.empty())
    {
        processLine(pending_);
        pending_.clear();
    }
    if (!dynamic_mode_)
    {
        emit();
    }
}

void Context::processLine(const std::string &line)
{
    if (!dynamic_mode_)
    {
        if (line == OPEN)
        {
            emit();
            dynamic_mode_ = true;
            depth_ = 1;
            return;
        }
        if (line == CLOSE)
        {
            return;
        }
        appendCommand(line);
    }
    else
    {
        if (line == OPEN)
        {
            ++depth_;
        }
        else if (line == CLOSE)
        {
            --depth_;
            if (depth_ == 0)
            {
                emit();
                dynamic_mode_ = false;
            }
        }
        else
        {
            appendCommand(line);
        }
    }
}

void Context::appendCommand(const std::string &line)
{
    if (current_.empty())
    {
        start_ = std::time(nullptr);
    }
    current_.push_back(line);
    if (!dynamic_mode_ && current_.size() == block_size_)
    {
        emit();
    }
}

void Context::emit()
{
    if (current_.empty())
    {
        return;
    }
    runtime_.submit(Block{current_, start_});
    current_.clear();
}

} // namespace async
