#include "TurnManager.hpp"

void TurnManager::advance()
{
    _current = (_current == PieceColor::White) ? PieceColor::Black : PieceColor::White;
}
