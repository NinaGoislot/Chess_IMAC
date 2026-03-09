#pragma once

#include <array>
#include "Case.hpp"

class Board {
public:
    static constexpr int SIZE = 8;

    Board();

    Case& getCase(int x, int y);
    const Case& getCase(int x, int y) const;


private:
    std::array<std::array<Case, SIZE>, SIZE> _cases;
};