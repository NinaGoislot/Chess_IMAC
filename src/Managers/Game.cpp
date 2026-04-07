#include "Managers/Game.hpp"
#include <imgui.h>
#include <array>
#include <memory>
#include "Game/PieceFactory.hpp"
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
    , _players{Player(PieceColor::White, "White"), Player(PieceColor::Black, "Black")}
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
    _currentTurn = PieceColor::White;
    placePieces();
}
//-------- DRAw --------

void Game::displayBoard()
{
    _boardRenderer.draw(_board, _settings, _currentTurn);
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

    Player& whitePlayer = _players[0];
    Player& blackPlayer = _players[1];

    placeBackRankPieces(0, whitePlayer);
    placeBackRankPieces(7, blackPlayer);
    for (int x = 0; x < Board::SIZE; ++x)
    {
        placePieceForPlayer(x, 1, PieceType::Pawn, whitePlayer);
        placePieceForPlayer(x, 6, PieceType::Pawn, blackPlayer);
    }
}

void Game::placePieceForPlayer(int x, int y, PieceType type, Player& owner)
{
    const PieceColor color   = owner.color();
    ImTextureID      texture = _textures.getPieceTexture(color, type);

    std::unique_ptr<Piece> piece = PieceFactory::create(type, color, texture);
    Piece*                 raw   = piece.get();

    _board.getCase(x, y).setPiece(std::move(piece));
    owner.addPiece(*raw);
}

void Game::placeBackRankPieces(int y, Player& owner)
{
    const std::array<PieceType, 8> order = {
        PieceType::Rook,
        PieceType::Knight,
        PieceType::Bishop,
        PieceType::Queen,
        PieceType::King,
        PieceType::Bishop,
        PieceType::Knight,
        PieceType::Rook,
    };

    for (int x = 0; x < Board::SIZE; ++x)
    {
        placePieceForPlayer(x, y, order[x], owner);
    }
}