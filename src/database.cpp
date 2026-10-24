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

Database::TableMap &Database::select(Table table)
{
    return table == Table::A ? a_ : b_;
}

const Database::TableMap &Database::select(Table table) const
{
    return table == Table::A ? a_ : b_;
}

}
