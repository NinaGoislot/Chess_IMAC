#pragma once

#include "Board/Board.hpp"
#include "utilities/MoveAttempt.hpp"

// Applies one move attempt to board state.
class MoveExecutor {
public:
    Board::MoveResult execute(Board& board, const MoveAttempt& attempt) const;
};
