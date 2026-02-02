#pragma once
#include "Board/Board.hpp"
// #include "Piece/Piece.hpp"

class ChessGame
{
public:
    ChessGame();
    ChessGame(ChessGame &&) = default;
    ChessGame(const ChessGame &) = default;
    ChessGame &operator=(ChessGame &&) = default;
    ChessGame &operator=(const ChessGame &) = default;
    ~ChessGame();

    void displayBoard();
    void placePieces();

private:
    Board _board;
};