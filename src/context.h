#pragma once

#include <cstddef>
#include <ctime>
#include <mutex>
#include <string>
#include <vector>

namespace async
{

class Runtime;

class Context
{
public:
    Context(std::size_t block_size, Runtime &runtime);

    void feed(const char *data, std::size_t size);
    void finish();

private:
    void processLine(const std::string &line);
    void appendCommand(const std::string &line);
    void emit();

    std::mutex mutex_;
    Runtime &runtime_;
    std::size_t block_size_;
    std::vector<std::string> current_;
    std::string pending_;
    std::time_t start_ = 0;
    int depth_ = 0;
    bool dynamic_mode_ = false;
};

} // namespace async
