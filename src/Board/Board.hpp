#pragma once

#include <array>
#include "Case.hpp"

class Board {
public:
    constexpr static int SIZE = 8;

    Board();

    void setCase(int x, int y, const Case& c)
    {
        _cases[x][y] = c;
    }

    // int getSize() const { return _size; }

    Case&       getCase(int x, int y);
    const Case& getCase(int x, int y) const;
    

private:
    std::array<std::array<Case, SIZE>, SIZE> _cases;
};