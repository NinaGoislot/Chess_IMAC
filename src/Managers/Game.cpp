#include "Managers/Game.hpp"
#include <imgui.h>
#include <array>
#include <memory>
#include "Game/settings.hpp"

// #include "quick_imgui/quick_imgui.hpp"

void Game::init()
{
    _renderer.initialize();
}

Game::Game()
    : _board()
    , _textures()
    , _renderer(_textures)
    , _players{}
{
    _textures.load();
    _players[0] = Player(PieceColor::White, "White", _textures);
    _players[1] = Player(PieceColor::Black, "Black", _textures);
    placePieces();
}

void Game::placePieces()
{
    _players[0].resetPieces();
    _players[1].resetPieces();

    // Pass the Y-coordinates for White (backrank 0, pawns 1)
    placePiecesForPlayer(0, 1, _players[0]);

    // Pass the Y-coordinates for Black (backrank 7, pawns 6)
    placePiecesForPlayer(7, 6, _players[1]);
}

void Game::displayBoard(const settings& gameSettings, float deltaTimeSeconds)
{
    _renderer.draw(_board, gameSettings, _currentTurn, deltaTimeSeconds);
}

void Game::placePiecesForPlayer(int backRankY, int pawnRankY, Player& owner)
{
    const auto& pieces = owner.getAllPieces();

    // We know pieces 0-7 are the back rank. Loop and place!
    for (int x = 0; x < Board::SIZE; ++x)
    {
        _board.getCase(x, backRankY).setPiece(pieces[x].get());
    }

    // We know pieces 8-15 are the pawns. Loop and place!
    for (int x = 0; x < Board::SIZE; ++x)
    {
        _board.getCase(x, pawnRankY).setPiece(pieces[x + 8].get());
    }
}
