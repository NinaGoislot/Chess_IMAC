#pragma once
#include "settings.hpp"
#include "Board/Board.hpp"
#include "Pieces/Piece.hpp"
#include "Render/TextureLoader.hpp"

class Game
{
public:
    Game();

    void displayBoard(const settings &s);
    void placePieces(const settings &s);

private:
    Board _board;
};