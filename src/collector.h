#pragma once

#include <cstddef>
#include <ctime>
#include <mutex>
#include <string>
#include <vector>

namespace async
{

class Runtime;

// Collects commands that belong to static blocks. Unlike Task 9, this
// collector is shared by every connection, so commands coming from different
// clients are mixed into the same static blocks.
class StaticCollector
{
public:
    StaticCollector(std::size_t block_size, Runtime &runtime);

    void add(const std::string &line);
    void flush();

    void connectionOpened();
    void connectionClosed();

private:
    void emit();

    std::mutex mutex_;
    Runtime &runtime_;
    std::size_t block_size_;
    std::vector<std::string> commands_;
    std::time_t start_ = 0;
    std::size_t active_connections_ = 0;
};

} // namespace async
