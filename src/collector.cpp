#include "collector.h"

#include "block.h"
#include "runtime.h"

namespace async
{

StaticCollector::StaticCollector(std::size_t block_size, Runtime &runtime)
    : runtime_(runtime), block_size_(block_size)
{
}

void StaticCollector::add(const std::string &line)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (commands_.empty())
    {
        start_ = std::time(nullptr);
    }
    commands_.push_back(line);
    if (commands_.size() == block_size_)
    {
        emit();
    }
}

void StaticCollector::flush()
{
    std::lock_guard<std::mutex> lock(mutex_);
    emit();
}

void StaticCollector::connectionOpened()
{
    std::lock_guard<std::mutex> lock(mutex_);
    ++active_connections_;
}

void StaticCollector::connectionClosed()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (active_connections_ > 0)
    {
        --active_connections_;
    }
    if (active_connections_ == 0)
    {
        emit();
    }
}

void StaticCollector::emit()
{
    if (commands_.empty())
    {
        return;
    }
    runtime_.submit(Block{commands_, start_});
    commands_.clear();
}

} // namespace async
