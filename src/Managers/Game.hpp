#pragma once

#include <vector>
#include "Board/Board.hpp"
#include "Game/Pieces/Piece.hpp"
#include "Game/Player.hpp"
#include "Game/settings.hpp"
#include "Render/BoardRenderer.hpp"

class Game
{
public:
    Game();

    void placePieces();
    void displayBoard(const settings& gameSettings);
    void init();

private:
    Board _board;
    TextureManager _textures;
    BoardRenderer _boardRenderer;
    std::vector<Player> _players;
};