#pragma once

#include <thread>

#include "block.h"
#include "blocking_queue.h"

namespace async
{

class Runtime
{
public:
    static Runtime &instance();

    void submit(const Block &block);

    ~Runtime();

private:
    Runtime();
    Runtime(const Runtime &) = delete;
    Runtime &operator=(const Runtime &) = delete;

    void logLoop();
    void fileLoop(int file_id);

    BlockingQueue<Block> console_queue_;
    BlockingQueue<Block> file_queue_;
    std::thread log_thread_;
    std::thread file1_thread_;
    std::thread file2_thread_;
};

} // namespace async
