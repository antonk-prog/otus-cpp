#pragma once

#include "iobserver.h"

class ConsoleWriter : public IObserver
{
public:
    void update(const Block &block) override;
};