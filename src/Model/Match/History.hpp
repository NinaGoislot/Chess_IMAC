#pragma once

#include <string>
#include <vector>

class MoveHistory {
public:
    void clear();
    void add(const std::string& entry);

    std::vector<std::string>&       entries();
    const std::vector<std::string>& entries() const;

private:
    std::vector<std::string> _entries;
};
