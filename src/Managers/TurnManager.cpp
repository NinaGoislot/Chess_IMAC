#include "TurnManager.hpp"

// Functions
void TurnManager::nextTurn()
{
    _current = (_current == PieceColor::White) ? PieceColor::Black : PieceColor::White;
}
