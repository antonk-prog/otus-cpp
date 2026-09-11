#include "bulk_parser.h"

#include <ctime>
#include <iostream>

namespace
{

    constexpr const char *OPEN = "{";
    constexpr const char *CLOSE = "}";

} // namespace

BulkParser::BulkParser(size_t block_size) : block_size_(block_size) {}

void BulkParser::attach(IObserver *observer)
{
    observers_.push_back(observer);
}

void BulkParser::detach(IObserver *observer)
{
    for (auto it = observers_.begin(); it != observers_.end(); ++it)
    {
        if (*it == observer)
        {
            observers_.erase(it);
            return;
        }
    }
}

void BulkParser::emit()
{
    if (current_.empty())
    {
        return;
    }
    const Block block{current_, start_};
    for (auto *observer : observers_)
    {
        observer->update(block);
    }
    current_.clear();
}

void BulkParser::run()
{
    std::string line;
    while (std::getline(std::cin, line))
    {
        if (!dynamic_)
        {
            if (line == OPEN)
            {
                emit();
                dynamic_ = true;
                depth_ = 1;
                continue;
            }
            if (line == CLOSE)
            {
                continue;
            }
            if (current_.empty())
            {
                start_ = std::time(nullptr);
            }
            current_.push_back(line);
            if (current_.size() == block_size_)
            {
                emit();
            }
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
                    dynamic_ = false;
                }
            }
            else
            {
                if (current_.empty())
                {
                    start_ = std::time(nullptr);
                }
                current_.push_back(line);
            }
        }
    }

    if (dynamic_)
    {
        return;
    }
    emit();
}