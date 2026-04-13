#include "Managers/Game.hpp"
#include <array>
#include <iostream>
#include <memory>
#include "Game/Chaos/ChaosMode.hpp"
#include "Game/settings.hpp"

// #include "quick_imgui/quick_imgui.hpp"

//-------- GET INSTANCE --------
Game& Game::instance()
{
    static Game instance;
    return instance;
}

//-------- CONSTRUCTOR --------
Game::Game()
    : _board()
    , _textures()
    , _renderer(_textures)
    , _players{}
    , _chaosMode(std::make_unique<ChaosMode>())
    , _promotionFlow(_board, _players, _turnManager, _textures)
{
    _board.setBeforeMoveHook([this](MoveAttempt& attempt) {
        return _chaosMode->beforeMove(attempt, _board, _players, _moveHistory);
    });
    _promotionFlow.setOnMoveValidated([this]() {
        _turnManager.advanceValidatedMove(
            [this](PieceColor currentTurn) {
                _chaosMode->onTurnEnd(_board, _players, _moveHistory, currentTurn);
            },
            [this](PieceColor currentTurn) {
                _chaosMode->onTurnStart(_board, _players, _moveHistory, currentTurn);
            }
        );
        onValidatedMoveAdvanced();
    });

    _textures.load();
    newGame(Mode::Classic);
}

Game::~Game() = default;

//-------- GETTERS --------

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

ChaosOptions& Game::getChaosOptionsMutable()
{
    return _chaosMode->getOptionsMutable();
}

const ChaosOptions& Game::getChaosOptions() const
{
    return _chaosMode->getOptions();
}

//-------- INIT --------

void Game::initialize(const AppConfig& config)
{
    _textures.load(config);
    _renderer.initialize(config);
    newGame();
}

void Game::newGame(Mode mode)
{
    _mode = mode;
    _chaosMode->setEnabled(_mode == Mode::Chaos);

    _board.clear();
    _turnManager.setCurrent(PieceColor::White);
    _turnCount  = 1;
    _players[0] = Player(PieceColor::White, "White", _textures);
    _players[1] = Player(PieceColor::Black, "Black", _textures);
    _moveHistory.clear();

    placePieces();

    _chaosMode->onGameSetup(_board, _players, _moveHistory, _turnManager.getCurrent());
    _chaosMode->onTurnStart(_board, _players, _moveHistory, _turnManager.getCurrent());

    std::cout << "Turn " << _turnCount << " - White to play\n";
}
//-------- DRAW --------

void Game::displayBoard()
{
    const PieceColor currentTurn = _turnManager.getCurrent();
    const auto       clickedCase = _renderer.draw(_board, _settings, currentTurn, ImGui::GetIO().DeltaTime, _chaosMode->getKirbyPosition());

    if (!clickedCase.has_value())
        return;

    if (_chaosMode->getHasKirbyAt(clickedCase->x, clickedCase->y))
        return;

    if (_board.onCaseClicked(clickedCase->x, clickedCase->y, currentTurn))
    {
        _turnManager.advanceValidatedMove(
            [this](PieceColor turn) {
                _chaosMode->onTurnEnd(_board, _players, _moveHistory, turn);
            },
            [this](PieceColor turn) {
                _chaosMode->onTurnStart(_board, _players, _moveHistory, turn);
            }
        );
        onValidatedMoveAdvanced();
        return;
    }

    if (_chaosMode->consumeSkipTurnRequested())
    {
        _moveHistory.push_back("Chaos: tour saute apres refus d'obeissance.");
        _turnManager.advanceValidatedMove(
            [this](PieceColor turn) {
                _chaosMode->onTurnEnd(_board, _players, _moveHistory, turn);
            },
            [this](PieceColor turn) {
                _chaosMode->onTurnStart(_board, _players, _moveHistory, turn);
            }
        );
        onValidatedMoveAdvanced();
    }
}

void Game::onValidatedMoveAdvanced()
{
    ++_turnCount;

    const char* side = (_turnManager.getCurrent() == PieceColor::White) ? "White" : "Black";
    std::cout << "Turn " << _turnCount << " - " << side << " to play\n";
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

//-------- FUNCTIONS --------

void Game::placePieces()
{
    _players[0].resetPieces();
    _players[1].resetPieces();

    // Pass the Y-coordinates for White (backrank 0, pawns 1)
    placePiecesForPlayer(7, 6, _players[0]);

    // Pass the Y-coordinates for Black (backrank 7, pawns 6)
    placePiecesForPlayer(0, 1, _players[1]);
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
