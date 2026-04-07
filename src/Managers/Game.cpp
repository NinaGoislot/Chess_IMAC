#include "Managers/Game.hpp"
#include <imgui.h>
#include <array>
#include <memory>
#include "Game/settings.hpp"

// #include "quick_imgui/quick_imgui.hpp"

//-------- GET INsTANCE --------
Game& Game::instance()
{
    static Game instance;
    return instance;
}

//-------- CONsTRUCTOR --------
Game::Game()
    : _board()
    , _textures()
    , _boardRenderer(_textures)
    , _players{}
{
    _textures.load();
    newGame();
}

//-------- GETTERs --------

settings& Game::getSettings()
{
    return _settings;
}

const settings& Game::getSettings() const
{
    return _settings;
}

const std::vector<std::string>& Game::getMoveHistory() const
{
    return _moveHistory;
}

//-------- INIT --------

void Game::newGame()
{
    _turnManager.setCurrent(PieceColor::White);
    _players[0] = Player(PieceColor::White, "White", _textures);
    _players[1] = Player(PieceColor::Black, "Black", _textures);
    placePieces();
}
//-------- DRAw --------

void Game::displayBoard()
{
    _boardRenderer.draw(_board, _settings);
}

//-------- ADD or UPDATE --------

void Game::addPlayerWhite(const std::string& name)
{
    _players[0].setName(name);
}

void Game::addPlayerBlack(const std::string& name)
{
    _players[1].setName(name);
}

void Game::addMoveToHistory(const std::string& move)
{
    _moveHistory.push_back(move);
}

//-------- FUNCTIONs --------

void Game::placePieces()
{
    _players[0].resetPieces();
    _players[1].resetPieces();

    // Pass the Y-coordinates for White (backrank 0, pawns 1)
    placePiecesForPlayer(0, 1, _players[0]);

    // Pass the Y-coordinates for Black (backrank 7, pawns 6)
    placePiecesForPlayer(7, 6, _players[1]);
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
