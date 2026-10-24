#pragma once

#include <map>
#include <shared_mutex>
#include <string>
#include <type_traits>
#include <utility>

namespace join
{

class Database
{
public:
    enum class Table
    {
        A,
        B
    };

    using TableMap = std::map<int, std::string>;

    bool insert(Table table, int id, const std::string &name);
    void truncate(Table table);

    template <typename JoinFn>
    std::invoke_result_t<JoinFn &, const TableMap &, const TableMap &>
    joinUnderSharedLock(JoinFn join) const
    {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return join(a_, b_);
    }

private:
    TableMap &select(Table table);
    const TableMap &select(Table table) const;

    mutable std::shared_mutex mutex_;
    TableMap a_;
    TableMap b_;
};

}
