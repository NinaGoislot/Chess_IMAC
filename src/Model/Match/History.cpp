#include "Model/Match/History.hpp"

void MoveHistory::clear()
{
    _entries.clear();
}

void MoveHistory::add(const std::string& entry)
{
    _entries.push_back(entry);
}

std::vector<std::string>& MoveHistory::entries()
{
    return _entries;
}

const std::vector<std::string>& MoveHistory::entries() const
{
    return _entries;
}
