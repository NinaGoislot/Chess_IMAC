#include "InputManager.hpp"
#include "Board/Board.hpp"
#include "Managers/Game.hpp"

InputManager& InputManager::instance()
{
    static InputManager instance;
    return instance;
}

// Functions
void InputManager::onCaseClicked(Board& board, int x, int y)
{
    TurnManager&     turnManager = Game::instance().turnManager();
    const PieceColor currentTurn = turnManager.getCurrent();
    const bool       moved       = board.onCaseClicked(x, y, currentTurn);
    if (moved)
    {
        turnManager.nextTurn();
    }
}
