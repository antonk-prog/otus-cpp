#include "database.h"

#include <mutex>

namespace join
{

bool Database::insert(Table table, int id, const std::string &name)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    auto &target = select(table);
    return target.emplace(id, name).second;
}

void Database::truncate(Table table)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    select(table).clear();
}

Database::Snapshot Database::snapshot() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return Snapshot{a_, b_};
}

std::map<int, std::string> &Database::select(Table table)
{
    return table == Table::A ? a_ : b_;
}

const std::map<int, std::string> &Database::select(Table table) const
{
    return table == Table::A ? a_ : b_;
}

}
