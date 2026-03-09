#pragma once

#include <vector>
#include "Board/Board.hpp"
#include "Game/Pieces/Piece.hpp"
#include "Game/Player.hpp"
#include "Game/settings.hpp"
#include "Render/TextureLoader.hpp"

class Game {
public:
    Game();
    void displayBoard(const settings& s);
    void placePieces(const settings& s);

    void init();

private:
    std::vector<Player> _players;
    Board               _board;
};