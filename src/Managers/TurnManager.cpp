#include "TurnManager.hpp"

// Functions
void TurnManager::nextTurn()
{
    _current = (_current == PieceColor::White) ? PieceColor::Black : PieceColor::White;
}

void TurnManager::advanceValidatedMove(const std::function<void(PieceColor)>& onTurnEnd, const std::function<void(PieceColor)>& onTurnStart)
{
    if (onTurnEnd)
        onTurnEnd(_current);

    nextTurn();

    if (onTurnStart)
        onTurnStart(_current);
}
