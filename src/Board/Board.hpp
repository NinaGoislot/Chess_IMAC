#pragma once

#include <array>
#include "Case.hpp"

class Board {
public:
    static constexpr int SIZE = 8;

    Board();

    Case&       getCase(int x, int y);
    const Case& getCase(int x, int y) const;
    void onCaseClicked(int x, int y);

    bool isInside(Vector2D pos) const;
    bool isEmpty(Vector2D pos) const;
    bool isEnemy(Vector2D pos, PieceColor color) const;


private:
    Case*                                    _selectedCase = nullptr;
    std::array<std::array<Case, SIZE>, SIZE> _cases;

    void clearHighlights();
};