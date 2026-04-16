#include "Model/Match/MatchState.hpp"
#include "utilities/PieceColorUtils.hpp"

namespace {

void appendTurnHistory(MoveHistory& history, int turnNumber, const std::string& playerName, PieceColor color)
{
    history.add(
        std::string("Tour ")
        + std::to_string(turnNumber)
        + " - A jouer: "
        + playerName
        + " ("
        + PieceColorUtils::toFrenchLabel(color)
        + ")"
    );
}

} // namespace

MatchState::MatchState(const TextureManager& textures)
    : _board()
    , _turnManager()
    , _players{}
    , _moveHistory()
    , _chaosMode(std::make_unique<ChaosMode>())
    , _textures(&textures)
{
    _players[0] = Player(PieceColor::White, "White", textures);
    _players[1] = Player(PieceColor::Black, "Black", textures);
}

/**
 *
 * Reinitialise la partie, les joueurs et les regles selon le mode choisi.
 * @param mode : mode de partie (classique ou chaos).
 * @return Aucun.
 */
void MatchState::newMatch(Mode mode)
{
    const std::string whiteName = _players[0].getName();
    const std::string blackName = _players[1].getName();

    _mode = mode;
    _chaosMode->setEnabled(_mode == Mode::Chaos);

    _board.clear();
    _turnManager.setCurrent(PieceColor::White);
    _promotion.clear();
    _moveHistory.clear();
    _validatedMoveCount = 0;
    _winner             = nullptr;

    _players[0] = Player(PieceColor::White, whiteName.empty() ? "White" : whiteName, *_textures);
    _players[1] = Player(PieceColor::Black, blackName.empty() ? "Black" : blackName, *_textures);

    placePieces();

    _chaosMode->onGameSetup(_board, _players, _moveHistory.entries(), _turnManager.getCurrent());
    _chaosMode->onTurnStart(_board, _players, _moveHistory.entries(), _turnManager.getCurrent());

    appendTurnHistory(_moveHistory, getCurrentTurnNumber(), getActivePlayerName(), _turnManager.getCurrent());
}

/**
 *
 * Tente de jouer un coup en appliquant regles, chaos et promotions.
 * @param from : position de depart.
 * @param to : position d'arrivee.
 * @return Vrai si le coup a ete valide.
 */
bool MatchState::tryMove(Vector2D from, Vector2D to)
{
    if (_winner != nullptr)
        return false;

    if (!_rules.isMoveValid(_board, from, to, _turnManager.getCurrent(), _promotion.getHasPending()))
        return false;

    MoveAttempt attempt;
    attempt.fromX       = static_cast<int>(from.getX());
    attempt.fromY       = static_cast<int>(from.getY());
    attempt.toX         = static_cast<int>(to.getX());
    attempt.toY         = static_cast<int>(to.getY());
    attempt.piece       = _board.getCase(attempt.fromX, attempt.fromY).getPiece();
    attempt.currentTurn = _turnManager.getCurrent();

    if (!applyChaosPreMove(attempt))
        return false;

    const Board::MoveResult result = _board.tryMove(attempt);
    if (!result.moved)
        return false;

    if (result.capturedPiece != nullptr && result.capturedPiece->getType() == PieceType::King)
    {
        const int winnerIndex = (result.movedPiece != nullptr && result.movedPiece->getColor() == PieceColor::White) ? 0 : 1;
        _winner               = &_players[winnerIndex];

        consumeCapturedPiece(result.capturedPiece);
        _moveHistory.add("Victoire de " + _winner->getName() + " : roi adverse capture.");
        return true;
    }

    if (_rules.isPromotion(result))
    {
        _promotion.start(result, _turnManager.getCurrent());
        return true;
    }

    consumeCapturedPiece(result.capturedPiece);
    applyTurnProgression();
    return true;
}

/**
 *
 * Valide une promotion en attente et applique la progression du tour.
 * @param type : type de piece choisie pour la promotion.
 * @return Vrai si la promotion est validee.
 */
bool MatchState::choosePromotion(PieceType type)
{
    if (_winner != nullptr)
        return false;

    if (!_promotion.resolve(_board, _players, type))
        return false;

    applyTurnProgression();
    return true;
}

void MatchState::cancelPendingPromotion()
{
    _promotion.cancel(_board);
}

bool MatchState::canSelect(Vector2D tile) const
{
    if (_winner != nullptr)
        return false;

    return _rules.canSelect(_board, tile, _turnManager.getCurrent(), _promotion.getHasPending());
}

std::vector<Vector2D> MatchState::getLegalMovesFrom(Vector2D from) const
{
    if (_winner != nullptr)
        return {};

    return _rules.getLegalMovesFrom(_board, from, _turnManager.getCurrent(), _promotion.getHasPending());
}

const Board& MatchState::getBoard() const
{
    return _board;
}

Board& MatchState::getBoard()
{
    return _board;
}

PieceColor MatchState::getCurrentTurn() const
{
    return _turnManager.getCurrent();
}

TurnManager& MatchState::getTurnManager()
{
    return _turnManager;
}

const TurnManager& MatchState::getTurnManager() const
{
    return _turnManager;
}

const std::vector<std::string>& MatchState::getMoveHistory() const
{
    return _moveHistory.entries();
}

const std::string& MatchState::getWhitePlayerName() const
{
    return _players[0].getName();
}

const std::string& MatchState::getBlackPlayerName() const
{
    return _players[1].getName();
}

const std::string& MatchState::getActivePlayerName() const
{
    return _turnManager.getCurrent() == PieceColor::White ? _players[0].getName() : _players[1].getName();
}

int MatchState::getFullTurnCount() const
{
    return _validatedMoveCount / 2;
}

int MatchState::getCurrentTurnNumber() const
{
    return getFullTurnCount() + 1;
}

bool MatchState::getHasWinner() const
{
    return _winner != nullptr;
}

const Player* MatchState::getWinner() const
{
    return _winner;
}

std::optional<std::pair<int, int>> MatchState::getKirbyPosition() const
{
    return _chaosMode->getKirbyPosition();
}

bool MatchState::getHasKirbyAt(int x, int y) const
{
    return _chaosMode->getHasKirbyAt(x, y);
}

bool MatchState::getHasPendingPromotion() const
{
    return _promotion.getHasPending();
}

std::optional<MatchState::PendingPromotionInfo> MatchState::getPendingPromotion() const
{
    return _promotion.getPendingInfo();
}

MatchState::Mode MatchState::getMode() const
{
    return _mode;
}

ChaosOptions& MatchState::getChaosOptionsMutable()
{
    return _chaosMode->getOptionsMutable();
}

const ChaosOptions& MatchState::getChaosOptions() const
{
    return _chaosMode->getOptions();
}

void MatchState::addPlayerWhite(const std::string& name)
{
    _players[0].setName(name);
}

void MatchState::addPlayerBlack(const std::string& name)
{
    _players[1].setName(name);
}

void MatchState::addMoveToHistory(const std::string& move)
{
    _moveHistory.add(move);
}

/**
 *
 * Applique les regles Chaos avant un coup, avec possibilite d'annulation.
 * @param attempt : tentative de coup a modifier ou annuler.
 * @return Vrai si le coup peut continuer.
 */
bool MatchState::applyChaosPreMove(MoveAttempt& attempt)
{
    if (_chaosMode->beforeMove(attempt, _board, _players, _moveHistory.entries()))
        return true;

    if (_chaosMode->consumeSkipTurnRequested())
    {
        _moveHistory.add("Chaos: tour saute apres refus d'obeissance.");
        applyTurnProgression();
    }

    return false;
}
void MatchState::consumeCapturedPiece(Piece* capturedPiece)
{
    if (capturedPiece == nullptr)
        return;

    const int ownerIndex = (capturedPiece->getColor() == PieceColor::White) ? 0 : 1;
    _players[ownerIndex].removePiece(*capturedPiece);
}

void MatchState::placePieces()
{
    _players[0].resetPieces();
    _players[1].resetPieces();

    placePiecesForPlayer(7, 6, _players[0]);
    placePiecesForPlayer(0, 1, _players[1]);
}

/**
 *
 * Place les pieces d'un joueur sur les rangs de depart.
 * @param backRankY : ligne des pieces majeures.
 * @param pawnRankY : ligne des pions.
 * @param owner : joueur proprietaire des pieces.
 * @return Aucun.
 */
void MatchState::placePiecesForPlayer(int backRankY, int pawnRankY, Player& owner)
{
    const auto& pieces = owner.getAllPieces();

    for (int x = 0; x < Board::SIZE; ++x)
    {
        _board.getCase(x, backRankY).setPiece(pieces[x].get());
    }

    for (int x = 0; x < Board::SIZE; ++x)
    {
        _board.getCase(x, pawnRankY).setPiece(pieces[x + 8].get());
    }
}

/**
 *
 * Avance le compteur de tour et declenche les hooks de fin/debut de tour.
 * @return Aucun.
 */
void MatchState::applyTurnProgression()
{
    ++_validatedMoveCount;

    _turnManager.advanceValidatedMove(
        [this](PieceColor turn) {
            _chaosMode->onTurnEnd(_board, _players, _moveHistory.entries(), turn);
        },
        [this](PieceColor turn) {
            _chaosMode->onTurnStart(_board, _players, _moveHistory.entries(), turn);
        }
    );

    appendTurnHistory(_moveHistory, getCurrentTurnNumber(), getActivePlayerName(), _turnManager.getCurrent());
}
