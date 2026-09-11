#pragma once

#include <cstddef>

#include "iobserver.h"

class FileWriter : public IObserver
{
public:
    void update(const Block &block) override;

private:
    size_t block_counter_ = 0;
};