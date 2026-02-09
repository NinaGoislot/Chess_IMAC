#pragma once
#include "_settings.hpp"
#include "Board/Board.hpp"
// #include "Piece/Piece.hpp"

class ChessGame
{
public:
    ChessGame();

    void displayBoard(const settings &s);
    void placePieces();

private:
    Board _board;
};