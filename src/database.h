#pragma once

#include <map>
#include <shared_mutex>
#include <string>

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



    struct Snapshot
    {
        std::map<int, std::string> a;
        std::map<int, std::string> b;
    };


    bool insert(Table table, int id, const std::string &name);
    void truncate(Table table);

    Snapshot snapshot() const;

private:
    std::map<int, std::string> &select(Table table);
    const std::map<int, std::string> &select(Table table) const;

    mutable std::shared_mutex mutex_;
    std::map<int, std::string> a_;
    std::map<int, std::string> b_;
};

}
