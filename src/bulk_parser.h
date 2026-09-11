#pragma once

#include <cstddef>
#include <vector>

#include "iobserver.h"

class BulkParser
{
public:
    explicit BulkParser(size_t block_size);

    void attach(IObserver *observer);
    void detach(IObserver *observer);
    void run();

private:
    void emit();

    size_t block_size_;
    std::vector<IObserver *> observers_;

    std::vector<std::string> current_;
    std::time_t start_ = 0;
    bool dynamic_ = false;
    int depth_ = 0;
};