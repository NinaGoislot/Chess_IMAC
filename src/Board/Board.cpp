#include "Board.hpp"


Board::Board() : _cases{} {
    for (int x{0}; x < SIZE; x++)
    {
        for (int y{0}; y < SIZE; y++)
        {
            // _cases[x][y] = Case(x, y);
            // setCase(x, y, Case(x, y));
            _cases[x][y] = Case(x, y);
        }
    }
}

Case&       Board::getCase(int x, int y) {
    return _cases[x][y];
}
const Case& Board::getCase(int x, int y) const {
    return _cases[x][y];
}