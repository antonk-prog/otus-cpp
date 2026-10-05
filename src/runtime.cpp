#include "runtime.h"

#include <atomic>
#include <fstream>
#include <iostream>
#include <string>

namespace async
{

namespace
{

std::atomic<unsigned long long> g_file_sequence{0};

std::string make_filename(const Block &block, int file_id)
{
    const unsigned long long sequence = g_file_sequence.fetch_add(1);
    return "bulk" + std::to_string(static_cast<long long>(block.timestamp)) +
           "_" + std::to_string(file_id) + "_" + std::to_string(sequence) +
           ".log";
}

} // namespace

Runtime &Runtime::instance()
{
    static Runtime runtime;
    return runtime;
}

Runtime::Runtime()
    : log_thread_(&Runtime::logLoop, this),
      file1_thread_(&Runtime::fileLoop, this, 1),
      file2_thread_(&Runtime::fileLoop, this, 2)
{
}

Runtime::~Runtime()
{
    console_queue_.close();
    file_queue_.close();

    if (log_thread_.joinable())
    {
        log_thread_.join();
    }
    if (file1_thread_.joinable())
    {
        file1_thread_.join();
    }
    if (file2_thread_.joinable())
    {
        file2_thread_.join();
    }
}

void Runtime::submit(const Block &block)
{
    console_queue_.push(block);
    file_queue_.push(block);
}

void Runtime::logLoop()
{
    Block block;
    while (console_queue_.pop(block))
    {
        std::cout << "bulk: ";
        for (std::size_t i = 0; i < block.commands.size(); ++i)
        {
            if (i != 0)
            {
                std::cout << ", ";
            }
            std::cout << block.commands[i];
        }
        std::cout << '\n';
        std::cout.flush();
    }
}

void Runtime::fileLoop(int file_id)
{
    Block block;
    while (file_queue_.pop(block))
    {
        std::ofstream out(make_filename(block, file_id));
        for (const auto &command : block.commands)
        {
            out << command << '\n';
        }
    }
}

} // namespace async
